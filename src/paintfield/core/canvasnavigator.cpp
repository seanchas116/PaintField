#include <QKeySequence>
#include <QMouseEvent>
#include <QTabletEvent>
#include "tabletevent.h"
#include "canvas.h"
#include "keytracker.h"
#include "appcontroller.h"
#include "settingsmanager.h"
#include "cursorstack.h"
#include "canvasview.h"

#include "canvasnavigator.h"

namespace PaintField {

struct CanvasNavigator::Data
{
	Canvas *canvas = 0;
	CanvasViewController *controller = 0;
	KeyTracker *keyTracker = 0;
	
	QKeySequence scaleKeys, rotationKeys, translationKeys;
	
	DragMode navigationMode = NoNavigation;
	QPoint navigationOrigin;
	
	double backupScale = 1, backupRotation = 0;
	QPoint backupTranslation;
	
	void backupTransforms()
	{
		backupScale = canvas->scale();
		backupRotation = canvas->rotation();
		backupTranslation = canvas->translation();
	}
	
	QPoint viewCenter() const
	{
		auto size = canvas->viewSize();
		return QPoint(size.width(), size.height()) / 2;
	}
};

CanvasNavigator::CanvasNavigator(KeyTracker *keyTracker, CanvasViewController *controller) :
	QObject(controller),
	d(new Data)
{
	d->canvas = controller->canvas();
	d->controller = controller;
	d->keyTracker = keyTracker;
	
	connect(d->keyTracker, SIGNAL(pressedKeysChanged(QSet<int>)), this, SLOT(onPressedKeysChanged()));
	
	// setup key bindings
	{
		auto keyBindingHash = appController()->settingsManager()->value({".key-bindings"}).toMap();
		
		d->translationKeys = keyBindingHash["paintfield.canvas.dragTranslation"].toString();
		d->scaleKeys = keyBindingHash["paintfield.canvas.dragScale"].toString();
		d->rotationKeys = keyBindingHash["paintfield.canvas.dragRotation"].toString();
	}
}

CanvasNavigator::~CanvasNavigator()
{
	delete d;
}

CanvasNavigator::DragMode CanvasNavigator::dragMode() const
{
	return d->navigationMode;
}

static const QString navigatingCursorId = "paintfield.canvas.navigate";

static const QString readyToTranslateCursorId = "paintfield.canvas.readyToTranslate";
static const QString readyToScaleCursorId = "paintfield.canvas.readyToScale";
static const QString readyToRotateCursorId = "paintfield.canvas.readyToRotate";

void CanvasNavigator::onPressedKeysChanged()
{
	PAINTFIELD_DEBUG;
	
	auto cursorStack = appController()->cursorStack();
	auto keyTracker = d->keyTracker;
	
	auto addOrRemove = [cursorStack, keyTracker](const QKeySequence &seq, const QString &id, const QCursor &cursor)
	{
		if (keyTracker->match(seq))
			cursorStack->add(id, cursor);
		else
			cursorStack->remove(id);
	};
	
	addOrRemove(d->translationKeys, readyToTranslateCursorId, Qt::OpenHandCursor);
	addOrRemove(d->scaleKeys, readyToScaleCursorId, Qt::SizeVerCursor);
	addOrRemove(d->rotationKeys, readyToRotateCursorId, Qt::OpenHandCursor);
}

void CanvasNavigator::mouseEvent(QMouseEvent *event)
{
	bool accepted;
	
	switch (event->type())
	{
		case QEvent::MouseButtonPress:
			
			emit clicked();
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

void CanvasNavigator::tabletEvent(QTabletEvent *event)
{
	bool accepted;
	
	switch (event->type())
	{
		case QEvent::TabletPress:
			
			emit clicked();
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

void CanvasNavigator::customTabletEvent(WidgetTabletEvent *event)
{
	bool accepted;
	
	switch (int(event->type()))
	{
		case EventWidgetTabletPress:
			
			emit clicked();
			accepted = tryBeginDragNavigation(event->posInt);
			break;
			
		case EventWidgetTabletMove:
			
			accepted = continueDragNavigation(event->posInt);
			break;
			
		case EventWidgetTabletRelease:
			
			accepted = endDragNavigation();
			break;
			
		default:
			
			accepted = false;
			break;
	}
	
	event->setAccepted(accepted);
}

void CanvasNavigator::wheelEvent(QWheelEvent *event)
{
	QPoint translation = d->canvas->translation();
	
	if (event->orientation() == Qt::Horizontal)
		translation += QPoint(event->delta(), 0);
	else
		translation += QPoint(0, event->delta());
	
	d->canvas->setTranslation(translation);
}

bool CanvasNavigator::tryBeginDragNavigation(const QPoint &pos)
{
	if (d->keyTracker->match(d->scaleKeys))
	{
		beginDragScaling(pos);
		return true;
	}
	if (d->keyTracker->match(d->rotationKeys))
	{
		beginDragRotation(pos);
		return true;
	}
	 if (d->keyTracker->match(d->translationKeys))
	{
		beginDragTranslation(pos);
		return true;
	}
	
	return false;
}

bool CanvasNavigator::continueDragNavigation(const QPoint &pos)
{
	switch (d->navigationMode)
	{
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

bool CanvasNavigator::endDragNavigation()
{
	switch (d->navigationMode)
	{
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

void CanvasNavigator::beginDragTranslation(const QPoint &pos)
{
	appController()->cursorStack()->add(navigatingCursorId, Qt::ClosedHandCursor);
	
	d->navigationMode = Translating;
	d->navigationOrigin = pos;
	
	d->backupTransforms();
}

void CanvasNavigator::continueDragTranslation(const QPoint &pos)
{
	d->canvas->setTranslation(d->backupTranslation + (pos - d->navigationOrigin));
}

void CanvasNavigator::endDragTranslation()
{
	appController()->cursorStack()->remove(navigatingCursorId);
	d->navigationMode = NoNavigation;
}

void CanvasNavigator::beginDragScaling(const QPoint &pos)
{
	appController()->cursorStack()->add(navigatingCursorId, Qt::SizeVerCursor);
	
	d->navigationMode = Scaling;
	d->navigationOrigin = pos;
	d->backupTransforms();
}

void CanvasNavigator::continueDragScaling(const QPoint &pos)
{
	auto delta = pos - d->navigationOrigin;
	
	constexpr double divisor = 100;
	
	
	double scaleRatio = exp2(-delta.y() / divisor);
	double scale = d->backupScale * scaleRatio;
	
	auto navigationOffset = d->navigationOrigin - d->viewCenter();
	
	auto translation = (d->backupTranslation - navigationOffset) * scaleRatio + navigationOffset;
	
	d->canvas->setScale(scale);
	d->canvas->setTranslation(translation);
}

void CanvasNavigator::endDragScaling()
{
	appController()->cursorStack()->remove(navigatingCursorId);
	d->navigationMode = NoNavigation;
}

void CanvasNavigator::beginDragRotation(const QPoint &pos)
{
	appController()->cursorStack()->add(navigatingCursorId, Qt::ClosedHandCursor);
	
	d->navigationMode = Rotating;
	d->navigationOrigin = pos;
	d->backupTransforms();
}

void CanvasNavigator::continueDragRotation(const QPoint &pos)
{
	auto originalDelta = d->navigationOrigin - d->viewCenter();
	auto delta = pos - d->viewCenter();
	if (originalDelta != QPoint() && delta != QPoint())
	{
		auto originalRotation = atan2(originalDelta.y(), originalDelta.x()) / M_PI * 180.0;
		auto deltaRotation = atan2(delta.y(), delta.x()) / M_PI * 180.0;
		
		QTransform transform;
		transform.rotate(deltaRotation - originalRotation);
		
		auto translation = d->backupTranslation * transform;
		d->canvas->setRotation(d->backupRotation + deltaRotation - originalRotation);
		d->canvas->setTranslation(translation);
	}
}

void CanvasNavigator::endDragRotation()
{
	appController()->cursorStack()->remove(navigatingCursorId);
	d->navigationMode = NoNavigation;
}

} // namespace PaintField
