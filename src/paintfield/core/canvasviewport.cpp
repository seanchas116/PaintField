#include "canvasviewport.h"

#include "appcontroller.h"
#include "canvasviewportstate.h"
#include "canvastooleventfilter.h"
#include "canvasnavigatoreventfilter.h"
#include "keytracker.h"
#include "tool.h"
#include "canvas.h"
#include "workspace.h"
#include "layerrenderer.h"
#include "document.h"
#include "layerscene.h"
#include "cursorstack.h"
#include "widgets/vanishingscrollbar.h"
#include "selection.h"

#include <QTimer>
#include <QPaintEvent>
#include <QPainter>

#include <boost/variant.hpp>

namespace PaintField {

struct CanvasViewport::Data
{
	CanvasViewport *mSelf = 0;

	Canvas *mCanvas = 0;

	bool mUpdateEnabled = true;
	bool mStroking = false;
	bool mStrokingOrToolEditing = false;

	QPointer<Tool> mTool;
	QCursor mToolCursor;

	CanvasViewportState mState;

	VanishingScrollBar *mScrollBarX = 0, *mScrollBarY = 0;

	void setTransforms(const SP<const CanvasTransforms> &transforms)
	{
		mState.setTransforms(transforms);

		auto maxAbsTranslation = mCanvas->maxAbsoluteTranslation();
		// update scroll bar range
		{
			mScrollBarX->setRange(0, 2 * maxAbsTranslation.x());
			mScrollBarY->setRange(0, 2 * maxAbsTranslation.y());

			mScrollBarX->setPageStep(mSelf->viewSize().width());
			mScrollBarY->setPageStep(mSelf->viewSize().height());
		}

		// update scroll bar value
		{
			mScrollBarX->setValue(maxAbsTranslation.x() - transforms->translation.x());
			mScrollBarY->setValue(maxAbsTranslation.y() - transforms->translation.y());
		}

		mSelf->update();
	}

	void setRetinaMode(bool mode)
	{
		emit mSelf->viewSizeChanged(mSelf->viewSize());
		mState.setRetinaMode(mode);
		mSelf->update();
	}

	void setDocumentSize(const QSize &size)
	{
		mState.setDocumentSize(size);
	}

	void setTool(Tool *tool)
	{
		mTool = tool;
		mState.setTool(tool);

		if (tool) {
			auto requestUpdateSet = static_cast<void(Tool::*)(const QPointSet &)>(&Tool::requestUpdate);
			auto requestUpdateHash = static_cast<void(Tool::*)(const QHash<QPoint,QRect> &)>(&Tool::requestUpdate);

			using namespace std::placeholders;
			connect(tool, requestUpdateSet, mSelf, std::bind(&Data::updateTiles, this, _1));
			connect(tool, requestUpdateHash, mSelf, std::bind(&Data::updateTiles, this, _1));
			connect(tool, &Tool::editingChanged, mSelf, std::bind(&Data::onStrokingOrToolEditingChanged, this));
			onStrokingOrToolEditingChanged();

			mToolCursor = tool->cursor();
		}
	}

	void onClicked()
	{
		mSelf->setFocus();
		mCanvas->workspace()->setCurrentCanvas(mCanvas);
	}

	void onStrokingOrToolEditingChanged()
	{
		if (!mTool)
			return;

		bool strokingOrToolEditing = mStroking || mTool->isEditing();

		if (mStrokingOrToolEditing != strokingOrToolEditing) {
			if (strokingOrToolEditing) {
				mCanvas->document()->layerScene()->abortThumbnailUpdate();
				mCanvas->disableUndoRedo();
			} else {
				mCanvas->enableUndoRedo();
			}
			mStrokingOrToolEditing = strokingOrToolEditing;
		}
	}

	void moveWidgets()
	{
		auto widgetRect = mSelf->geometry();

		int barWidthX = mScrollBarX->totalBarWidth();
		int barWidthY = mScrollBarY->totalBarWidth();

		auto scrollBarXRect = widgetRect.adjusted(0, widgetRect.height() - barWidthX, -barWidthY, 0);
		auto scrollBarYRect = widgetRect.adjusted(widgetRect.width() - barWidthY, 0, 0, -barWidthX);

		mScrollBarX->setGeometry(scrollBarXRect);
		mScrollBarY->setGeometry(scrollBarYRect);
	}

	/// set focus on the viewport and make canvas current
	void activate()
	{
		mSelf->setFocus();
		mCanvas->workspace()->setCurrentCanvas(mCanvas);
	}

	void updateSelectionTiles(const SelectionSurface &surface, const QPointSet &keys)
	{
		if (mUpdateEnabled)
			mSelf->repaint(mState.updateSelectionTiles(surface, keys));
	}

	void updateTiles(const boost::variant<QPointSet, QHash<QPoint, QRect>> &keysOrRectForKeys)
	{
		if (mUpdateEnabled)
			mSelf->repaint(mState.updateTiles(keysOrRectForKeys));
	}
};

CanvasViewportSurface CanvasViewport::mergedSurface() const
{
	return d->mState.mergedSurface();
}

QSize CanvasViewport::viewSize() const
{
	auto size = this->size();
	if (d->mCanvas->isRetinaMode())
		size = QSize(size.width() * 2, size.height() * 2);
	return size;
}

void CanvasViewport::setCanvasUpdatesEnabled(bool x)
{
	d->mUpdateEnabled = x;
}

bool CanvasViewport::isCanvasUpdatesEnabled() const
{
	return d->mUpdateEnabled;
}

void CanvasViewport::resizeEvent(QResizeEvent *)
{
	d->moveWidgets();
	emit viewSizeChanged(viewSize());
}

static const QString toolCursorId = "paintfield.canvas.tool";

void CanvasViewport::enterEvent(QEvent *)
{
	d->activate();
	appController()->cursorStack()->add(toolCursorId, d->mToolCursor);
}

void CanvasViewport::leaveEvent(QEvent *)
{
	appController()->cursorStack()->remove(toolCursorId);
}

void CanvasViewport::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setCompositionMode(QPainter::CompositionMode_Source);

	d->mState.render(&painter, event->rect());
}

bool CanvasViewport::event(QEvent *event)
{
	switch (event->type()) {
		case QEvent::TabletPress:
		case QEvent::MouseButtonPress:
			d->mStroking = true;
			d->onStrokingOrToolEditingChanged();
			break;
		case QEvent::TabletRelease:
		case QEvent::MouseButtonRelease:
			d->mStroking = false;
			d->onStrokingOrToolEditingChanged();
			break;
		default:
			break;
	}
	return QWidget::event(event);
}

CanvasViewport::CanvasViewport(Canvas *canvas, QWidget *parent) :
	QWidget(parent),
	d(new Data)
{
	using namespace std::placeholders;

	d->mSelf = this;
	d->mCanvas = canvas;
	d->mState.setCanvas(canvas);

	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_OpaquePaintEvent);

	// setup objects

	auto keyTracker = new KeyTracker(this);
	auto eventSender = new CanvasToolEventFilter(canvas, this);
	auto navigator = new CanvasNavigatorEventFilter(keyTracker, canvas, this);
	connect(navigator, SIGNAL(clicked()), this, SLOT(setFocus()));

	installEventFilter(eventSender);
	installEventFilter(navigator);
	installEventFilter(keyTracker);

	// setup scrollbars
	{
		auto sx = new VanishingScrollBar(Qt::Horizontal, this);
		auto sy = new VanishingScrollBar(Qt::Vertical, this);

		auto onScrollBarXChanged = [this](int x) {
			if (d->mCanvas->isRetinaMode())
				x *= 2;
			d->mCanvas->setTranslationX(d->mCanvas->maxAbsoluteTranslation().x() - x);
		};

		auto onScrollBarYChanged = [this](int y) {
			if (d->mCanvas->isRetinaMode())
				y *= 2;
			d->mCanvas->setTranslationY(d->mCanvas->maxAbsoluteTranslation().y() - y);
		};

		connect(sx, &VanishingScrollBar::sliderMoved, this, onScrollBarXChanged);
		connect(sy, &VanishingScrollBar::sliderMoved, this, onScrollBarYChanged);
		connect(sx, SIGNAL(valueChanged(int)), sy, SLOT(wakeUp()));
		connect(sy, SIGNAL(valueChanged(int)), sx, SLOT(wakeUp()));

		d->mScrollBarX = sx;
		d->mScrollBarY = sy;

		d->moveWidgets();
	}

	// connect to canvas
	{
		connect(canvas, &Canvas::transformsChanged, this, std::bind(&Data::setTransforms, d.data(), _1));
		d->setTransforms(canvas->transforms());

		auto onToolChanged = [this, eventSender](Tool *tool) {
			d->setTool(tool);
			eventSender->setTool(tool);
		};
		connect(canvas, &Canvas::toolChanged, this, onToolChanged);
		onToolChanged(canvas->tool());

		connect(this, &CanvasViewport::viewSizeChanged, canvas, &Canvas::setViewSize);
		canvas->setViewSize(this->viewSize());

		connect(canvas, &Canvas::retinaModeChanged, this, std::bind(&Data::setRetinaMode, d.data(), _1));
		d->setRetinaMode(canvas->isRetinaMode());

		connect(canvas->document(), &Document::sizeChanged, this, std::bind(&Data::setDocumentSize, d.data(), _1));
		d->setDocumentSize(canvas->document()->size());

		auto setFocusIfCanvasSame = [this](Canvas *canvas) {
			if (d->mCanvas == canvas)
				setFocus();
		};
		connect(canvas->workspace(), &Workspace::currentCanvasChanged, this, setFocusIfCanvasSame);
		setFocusIfCanvasSame(canvas->workspace()->currentCanvas());

		connect(canvas->document()->selection(), &Selection::surfaceChanged, this, std::bind(&Data::updateSelectionTiles, d.data(), _1, _2));
		d->updateSelectionTiles(canvas->document()->selection()->surface(), canvas->document()->tileKeys());
	}
	connect(canvas->document()->layerScene(), &LayerScene::tilesUpdated, this, std::bind(&Data::updateTiles, d.data(), _1));
	d->updateTiles(canvas->document()->tileKeys());
}

CanvasViewport::~CanvasViewport()
{
}

} // namespace PaintField
