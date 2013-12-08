#include <QKeySequence>
#include <QMouseEvent>
#include <QTabletEvent>
#include "canvascursorevent.h"
#include "canvas.h"
#include "keytracker.h"
#include "appcontroller.h"
#include "settingsmanager.h"
#include "cursorstack.h"
#include "canvasview.h"

#include "canvasnavigatoreventfilter.h"

namespace PaintField {

struct CanvasNavigatorEventFilter::Data
{
	CanvasNavigatorEventFilter *mSelf = 0;

	Canvas *mCanvas = 0;
	KeyTracker *mKeyTracker = 0;
	
	QKeySequence mScaleKeys, mRotationKeys, mTranslationKeys;
	
	DragMode mNavigationMode = NoNavigation;
	QPoint mNavigationOrigin;
	
	double mMemorizedScale = 1, mMemorizedRotation = 0;
	QPoint mMemorizedTranslation;
	
	void backupTransforms()
	{
		mMemorizedScale = mCanvas->scale();
		mMemorizedRotation = mCanvas->rotation();
		mMemorizedTranslation = mCanvas->translation();
	}
	
	QPoint viewCenter() const
	{
		auto size = mCanvas->viewSize();
		return QPoint(size.width(), size.height()) / 2;
	}

	void onPressedKeysChanged();

	void mouseEvent(QMouseEvent *event);
	void tabletEvent(QTabletEvent *event);
	void wheelEvent(QWheelEvent *event);

	bool tryBeginDragNavigation(const QPoint &pos);
	bool continueDragNavigation(const QPoint &pos);
	bool endDragNavigation();

	void beginDragTranslation(const QPoint &pos);
	void continueDragTranslation(const QPoint &pos);
	void endDragTranslation();

	void beginDragScaling(const QPoint &pos);
	void continueDragScaling(const QPoint &pos);
	void endDragScaling();

	void beginDragRotation(const QPoint &pos);
	void continueDragRotation(const QPoint &pos);
	void endDragRotation();
};

CanvasNavigatorEventFilter::CanvasNavigatorEventFilter(KeyTracker *keyTracker, Canvas *canvas, QObject *parent) :
	QObject(parent),
	d(new Data)
{
	d->mSelf = this;
	d->mCanvas = canvas;
	d->mKeyTracker = keyTracker;
	
	connect(d->mKeyTracker, &KeyTracker::pressedKeysChanged, this, std::bind(&Data::onPressedKeysChanged, d.data()));
	
	// setup key bindings
	{
		auto keyBindingHash = appController()->settingsManager()->value({".key-bindings"}).toMap();
		
		d->mTranslationKeys = keyBindingHash["paintfield.canvas.dragTranslation"].toString();
		d->mScaleKeys = keyBindingHash["paintfield.canvas.dragScale"].toString();
		d->mRotationKeys = keyBindingHash["paintfield.canvas.dragRotation"].toString();
	}
}

CanvasNavigatorEventFilter::~CanvasNavigatorEventFilter()
{
}

CanvasNavigatorEventFilter::DragMode CanvasNavigatorEventFilter::dragMode() const
{
	return d->mNavigationMode;
}

static const QString navigatingCursorId = "paintfield.canvas.navigate";

static const QString readyToTranslateCursorId = "paintfield.canvas.readyToTranslate";
static const QString readyToScaleCursorId = "paintfield.canvas.readyToScale";
static const QString readyToRotateCursorId = "paintfield.canvas.readyToRotate";

void CanvasNavigatorEventFilter::Data::onPressedKeysChanged()
{
	PAINTFIELD_DEBUG;
	
	auto cursorStack = appController()->cursorStack();
	auto keyTracker = mKeyTracker;
	
	auto addOrRemove = [cursorStack, keyTracker](const QKeySequence &seq, const QString &id, const QCursor &cursor) {
		if (keyTracker->matches(seq))
			cursorStack->add(id, cursor);
		else
			cursorStack->remove(id);
	};
	
	addOrRemove(mTranslationKeys, readyToTranslateCursorId, Qt::OpenHandCursor);
	addOrRemove(mScaleKeys, readyToScaleCursorId, Qt::SizeVerCursor);
	addOrRemove(mRotationKeys, readyToRotateCursorId, Qt::OpenHandCursor);
}

bool CanvasNavigatorEventFilter::eventFilter(QObject *object, QEvent *event)
{
	Q_UNUSED(object);
	switch (event->type()) {
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		case QEvent::MouseMove:
			d->mouseEvent(static_cast<QMouseEvent *>(event));
			return event->isAccepted();
		case QEvent::TabletPress:
		case QEvent::TabletRelease:
		case QEvent::TabletMove:
			d->tabletEvent(static_cast<QTabletEvent *>(event));
			return event->isAccepted();
		case QEvent::Wheel:
			d->wheelEvent(static_cast<QWheelEvent *>(event));
			return event->isAccepted();
		default:
			return false;
	}
}

void CanvasNavigatorEventFilter::Data::mouseEvent(QMouseEvent *event)
{
	bool accepted;

	switch (event->type()) {
		case QEvent::MouseButtonPress:
			emit mSelf->clicked();
			accepted = tryBeginDragNavigation(event->pos());
			break;
		case QEvent::MouseMove:
			accepted = continueDragNavigation(event->pos());
			break;
		case QEvent::MouseButtonRelease:
			accepted = endDragNavigation();
			break;
		default:
			accepted = false;
			break;
	}
	
	event->setAccepted(accepted);
}

void CanvasNavigatorEventFilter::Data::tabletEvent(QTabletEvent *event)
{
	bool accepted;
	
	switch (event->type()) {
		case QEvent::TabletPress:
			emit mSelf->clicked();
			accepted = tryBeginDragNavigation(event->pos());
			break;
		case QEvent::TabletMove:
			accepted = continueDragNavigation(event->pos());
			break;
		case QEvent::TabletRelease:
			accepted = endDragNavigation();
			break;
		default:
			accepted = false;
			break;
	}
	
	event->setAccepted(accepted);
}

void CanvasNavigatorEventFilter::Data::wheelEvent(QWheelEvent *event)
{
	QPoint translation = mCanvas->translation();
	
	if (event->orientation() == Qt::Horizontal)
		translation += QPoint(event->delta(), 0);
	else
		translation += QPoint(0, event->delta());
	
	mCanvas->setTranslation(translation);
}

bool CanvasNavigatorEventFilter::Data::tryBeginDragNavigation(const QPoint &pos)
{
	if (mKeyTracker->matches(mScaleKeys)) {
		beginDragScaling(pos);
		return true;
	}
	if (mKeyTracker->matches(mRotationKeys)) {
		beginDragRotation(pos);
		return true;
	}
	 if (mKeyTracker->matches(mTranslationKeys)) {
		beginDragTranslation(pos);
		return true;
	}
	
	return false;
}

bool CanvasNavigatorEventFilter::Data::continueDragNavigation(const QPoint &pos)
{
	switch (mNavigationMode) {
		default:
		case NoNavigation:
			return false;
		case Translating:
			continueDragTranslation(pos);
			return true;
		case Scaling:
			continueDragScaling(pos);
			return true;
		case Rotating:
			continueDragRotation(pos);
			return true;
	}
}

bool CanvasNavigatorEventFilter::Data::endDragNavigation()
{
	switch (mNavigationMode) {
		default:
		case NoNavigation:
			return false;
		case Translating:
			endDragTranslation();
			return true;
		case Scaling:
			endDragScaling();
			return true;
		case Rotating:
			endDragRotation();
			return true;
	}
}

void CanvasNavigatorEventFilter::Data::beginDragTranslation(const QPoint &pos)
{
	appController()->cursorStack()->add(navigatingCursorId, Qt::ClosedHandCursor);
	
	mNavigationMode = Translating;
	mNavigationOrigin = pos;
	
	this->backupTransforms();
}

void CanvasNavigatorEventFilter::Data::continueDragTranslation(const QPoint &pos)
{
	mCanvas->setTranslation(mMemorizedTranslation + (pos - mNavigationOrigin));
}

void CanvasNavigatorEventFilter::Data::endDragTranslation()
{
	appController()->cursorStack()->remove(navigatingCursorId);
	mNavigationMode = NoNavigation;
}

void CanvasNavigatorEventFilter::Data::beginDragScaling(const QPoint &pos)
{
	appController()->cursorStack()->add(navigatingCursorId, Qt::SizeVerCursor);
	
	mNavigationMode = Scaling;
	mNavigationOrigin = pos;
	this->backupTransforms();
}

void CanvasNavigatorEventFilter::Data::continueDragScaling(const QPoint &pos)
{
	auto delta = pos - mNavigationOrigin;
	
	constexpr double divisor = 100;
	
	double scaleRatio = exp2(-delta.y() / divisor);
	double scale = mMemorizedScale * scaleRatio;
	
	auto navigationOffset = mNavigationOrigin - this->viewCenter();
	
	auto translation = (mMemorizedTranslation - navigationOffset) * scaleRatio + navigationOffset;
	
	mCanvas->setScale(scale);
	mCanvas->setTranslation(translation);
}

void CanvasNavigatorEventFilter::Data::endDragScaling()
{
	appController()->cursorStack()->remove(navigatingCursorId);
	mNavigationMode = NoNavigation;
}

void CanvasNavigatorEventFilter::Data::beginDragRotation(const QPoint &pos)
{
	appController()->cursorStack()->add(navigatingCursorId, Qt::ClosedHandCursor);
	
	mNavigationMode = Rotating;
	mNavigationOrigin = pos;
	this->backupTransforms();
}

void CanvasNavigatorEventFilter::Data::continueDragRotation(const QPoint &pos)
{
	auto originalDelta = mNavigationOrigin - this->viewCenter();
	auto delta = pos - this->viewCenter();

	if (originalDelta != QPoint() && delta != QPoint()) {

		auto originalRotation = atan2(originalDelta.y(), originalDelta.x()) / M_PI * 180.0;
		auto deltaRotation = atan2(delta.y(), delta.x()) / M_PI * 180.0;
		
		QTransform transform;
		transform.rotate(deltaRotation - originalRotation);
		
		auto translation = mMemorizedTranslation * transform;
		mCanvas->setRotation(mMemorizedRotation + deltaRotation - originalRotation);
		mCanvas->setTranslation(translation);
	}
}

void CanvasNavigatorEventFilter::Data::endDragRotation()
{
	appController()->cursorStack()->remove(navigatingCursorId);
	mNavigationMode = NoNavigation;
}

} // namespace PaintField
