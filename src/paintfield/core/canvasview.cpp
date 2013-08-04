#include <QTimer>
#include <QPointer>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include "widgets/vanishingscrollbar.h"
#include "canvasnavigator.h"
#include "canvastooleventsender.h"
#include "canvas.h"
#include "document.h"
#include "layerscene.h"
#include "keytracker.h"
#include "appcontroller.h"
#include "tool.h"
#include "workspace.h"
#include "cursorstack.h"
#include "canvasviewportcontroller.h"

#include "canvasview.h"

using namespace Malachite;

namespace PaintField {

void CanvasView::paintEvent(QPaintEvent *)
{
	PAINTFIELD_DEBUG << "paintevent";
}

void CanvasView::resizeEvent(QResizeEvent *)
{
	emit resized(size());
	emit moved();
}

void CanvasView::changeEvent(QEvent *ev)
{
	switch (ev->type())
	{
		case QEvent::ParentChange:
		case QEvent::EnabledChange:
			emit moved();
		default:
			break;
	}
}

void CanvasView::showEvent(QShowEvent *)
{
	emit moved();
}

void CanvasView::hideEvent(QHideEvent *)
{
	emit moved();
}

class CanvasRenderer : public LayerRenderer
{
public:
	CanvasRenderer() : LayerRenderer() {}
	
	void setTool(Tool *tool) { _tool = tool; }
	
protected:
	
	void drawLayer(SurfacePainter *painter, const LayerConstRef &layer) override
	{
		if (_tool && _tool->layerDelegations().contains(layer))
			_tool->drawLayer(painter, layer);
		else
			LayerRenderer::drawLayer(painter, layer);
	}
	
	void renderChildren(SurfacePainter *painter, const LayerConstRef &parent) override
	{
		if (!_tool || _tool->layerInsertions().isEmpty())
		{
			LayerRenderer::renderChildren(painter, parent);
		}
		else
		{
			auto originalLayers = parent->children();
			auto layers = originalLayers;
			
			for (auto insertion : _tool->layerInsertions())
			{
				if (insertion.parent == parent)
				{
					int index = insertion.index;
					auto layer = insertion.layer;
					if (index == originalLayers.size())
					{
						layers << layer;
					}
					else
					{
						auto layerAt = originalLayers.at(index);
						int trueIndex = layers.indexOf(layerAt);
						layers.insert(trueIndex, layer);
					}
				}
			}
			
			renderLayers(painter, layers);
		}
	}
	
private:
	
	Tool *_tool = 0;
};

struct CanvasViewController::Data
{
	Canvas *canvas = 0;
	
	// tool
	QPointer<Tool> tool;
	QCursor toolCursor;
	
	// widgets
	CanvasView *view = 0;
	QGraphicsView *graphicsView = 0;
	VanishingScrollBar *scrollBarX = 0, *scrollBarY = 0;
	
	// other objects
	CanvasViewportController *viewportController = 0;
	KeyTracker *keyTracker = 0;
	QGraphicsScene *graphicsScene = 0;
	QTimer *accurateUpdateTimer = 0;
	CanvasToolEventSender *eventSender = 0;
	CanvasNavigator *navigator = 0;
	
	// other
	bool updateEnabled = true;
	bool graphicsViewDraggingItem = false;
	bool stroking = false;
	bool strokingOrToolEditing = false;
};

CanvasViewController::CanvasViewController(Canvas *canvas) :
	QObject(canvas),
	d(new Data)
{
	d->canvas = canvas;
	
	// setup objects
	{
		d->keyTracker = new KeyTracker(this);
		d->graphicsScene = new QGraphicsScene(this);
		d->eventSender = new CanvasToolEventSender(this);
		d->navigator = new CanvasNavigator(d->keyTracker, this);
		
		{
			auto timer = new QTimer(this);
			timer->setSingleShot(true);
			timer->setInterval(500);
			connect(timer, SIGNAL(timeout()), this, SLOT(updateViewportAccurately()));
			d->accurateUpdateTimer = timer;
		}
	}
	
	// setup views
	{
		auto view = new CanvasView(canvas);
		d->view = view;
		view->setMouseTracking(true);
		connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)), view, SLOT(deleteLater()));
		connect(view, SIGNAL(resized(QSize)), this, SLOT(onViewWidgetSizeChanged()));
	
		// setup viewport
		{
			auto vp = new CanvasViewportController(this);
			d->viewportController = vp;
			vp->viewport()->setFocusPolicy(Qt::ClickFocus);
			connect(vp, SIGNAL(viewSizeChanged(QSize)), canvas, SLOT(setViewSize(QSize)));
			connect(canvas, SIGNAL(transformsChanged(Ref<const CanvasTransforms>)), vp, SLOT(setTransforms(Ref<const CanvasTransforms>)));
			connect(canvas, SIGNAL(retinaModeChanged(bool)), vp, SLOT(setRetinaMode(bool)));
			vp->setTransforms(canvas->transforms());
			vp->setRetinaMode(canvas->isRetinaMode());
			vp->setDocumentSize(canvas->document()->size());
			connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)), vp, SLOT(deleteViewportLater()));
		}
		
		d->graphicsView = new QGraphicsView(view);
		d->graphicsView->setStyleSheet("QGraphicsView { border-style: none; background: transparent; }");
		d->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		d->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		d->graphicsView->setRenderHint(QPainter::Antialiasing, true);
		d->graphicsView->setScene(d->graphicsScene);
		d->graphicsView->setInteractive(true);
		
#ifdef PF_CANVAS_VIEWPORT_COCOA
		d->graphicsView->setAttribute(Qt::WA_NativeWindow);
#endif
		
		// setup scrollbars
		{
			d->scrollBarX = new VanishingScrollBar(Qt::Horizontal, viewport());
			d->scrollBarY = new VanishingScrollBar(Qt::Vertical, viewport());
			
			connect(d->scrollBarX, SIGNAL(sliderMoved(int)), this, SLOT(onScrollBarXChanged(int)));
			connect(d->scrollBarY, SIGNAL(sliderMoved(int)), this, SLOT(onScrollBarYChanged(int)));
			connect(d->scrollBarX, SIGNAL(valueChanged(int)), d->scrollBarY, SLOT(wakeUp()));
			connect(d->scrollBarY, SIGNAL(valueChanged(int)), d->scrollBarX, SLOT(wakeUp()));
			
#ifdef PF_CANVAS_VIEWPORT_COCOA
			d->scrollBarX->setAttribute(Qt::WA_NativeWindow);
			d->scrollBarY->setAttribute(Qt::WA_NativeWindow);
			connect(d->graphicsScene, SIGNAL(changed(QList<QRectF>)), d->scrollBarX, SLOT(update()));
			connect(d->graphicsScene, SIGNAL(changed(QList<QRectF>)), d->scrollBarY, SLOT(update()));
#endif
		}
		
		viewport()->installEventFilter(this);
		d->graphicsView->viewport()->installEventFilter(this);
		
		connect(view, SIGNAL(moved()), this, SLOT(moveWidgets()));
	}
	
	moveWidgets();
	
	// connect to canvas
	{
		connect(canvas, SIGNAL(transformsChanged(Ref<const CanvasTransforms>)), this, SLOT(onTransformUpdated()));
		onTransformUpdated();
		
		connect(canvas, SIGNAL(toolChanged(Tool*)), this, SLOT(setTool(Tool*)));
		setTool(canvas->tool());
		
		canvas->setViewController(this);
		
		connect(this, SIGNAL(viewSizeChanged(QSize)), canvas, SLOT(setViewSize(QSize)));
		canvas->setViewSize(viewSize());
		connect(canvas, SIGNAL(retinaModeChanged(bool)), this, SLOT(onRetinaModeChanged(bool)));
		
		connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)), this, SLOT(onCanvasWillBeDeleted()));
		connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)), this, SLOT(deleteLater()));
		
		connect(canvas->workspace(), SIGNAL(currentCanvasChanged(Canvas*)), this, SLOT(onCurrentCanvasChanged(Canvas*)));
		onCurrentCanvasChanged(canvas->workspace()->currentCanvas());
	}
	
	connect(canvas->document()->layerScene(), SIGNAL(tilesUpdated(QPointSet)), this, SLOT(updateTiles(QPointSet)));
	updateTiles(canvas->document()->tileKeys());
}

CanvasViewController::~CanvasViewController()
{
	delete d;
}

CanvasView *CanvasViewController::view()
{
	return d->view;
}

bool CanvasViewController::isUpdateTilesEnabled() const
{
	return d->updateEnabled;
}

Canvas *CanvasViewController::canvas()
{
	return d->canvas;
}

QSize CanvasViewController::viewSize()
{
	auto size = d->view->size();
	if (d->canvas->isRetinaMode())
		size = QSize(size.width() * 2, size.height() * 2);
	return size;
}

QWidget *CanvasViewController::viewport()
{
	return d->viewportController->viewport();
}

void CanvasViewController::setUpdateTilesEnabled(bool enable)
{
	d->updateEnabled = enable;
}

void CanvasViewController::setTool(Tool *tool)
{
	if (d->tool && d->tool->graphicsItem())
	{
		d->graphicsScene->removeItem(d->tool->graphicsItem());
	}
	
	d->tool = tool;
	d->eventSender->setTool(tool);
	
	if (tool)
	{
		connect(tool, SIGNAL(requestUpdate(QPointSet)), this, SLOT(updateTiles(QPointSet)));
		connect(tool, SIGNAL(requestUpdate(QHash<QPoint,QRect>)), this, SLOT(updateTiles(QHash<QPoint,QRect>)));
		connect(tool, SIGNAL(editingChanged(bool)), this, SLOT(onStrokingOrToolEditingChanged()));
		onStrokingOrToolEditingChanged();
		
		d->toolCursor = tool->cursor();
		
		if (tool->graphicsItem())
		{
			d->graphicsScene->addItem(tool->graphicsItem());
			d->graphicsView->setVisible(true);
			d->scrollBarX->raise();
			d->scrollBarY->raise();
		}
		else
		{
			d->graphicsView->setVisible(false);
		}
	}
}

void CanvasViewController::updateViewportAccurately()
{
	if (d->navigator->dragMode() != CanvasNavigator::NoNavigation)
		return;
	
	d->viewportController->update();
}

void CanvasViewController::onClicked()
{
	d->view->setFocus();
	d->canvas->workspace()->setCurrentCanvas(d->canvas);
}

void CanvasViewController::onScrollBarXChanged(int x)
{
	if (d->canvas->isRetinaMode())
		x *= 2;
	
	d->canvas->setTranslationX(d->canvas->maxAbsoluteTranslation().x() - x);
}

void CanvasViewController::onScrollBarYChanged(int y)
{
	if (d->canvas->isRetinaMode())
		y *= 2;
	
	d->canvas->setTranslationY(d->canvas->maxAbsoluteTranslation().y() - y);
}

void CanvasViewController::updateScrollBarRange()
{
	auto maxAbsTranslation = d->canvas->maxAbsoluteTranslation();
	
	d->scrollBarX->setRange(0, 2 * maxAbsTranslation.x());
	d->scrollBarY->setRange(0, 2 * maxAbsTranslation.y());
	
	d->scrollBarX->setPageStep(viewSize().width());
	d->scrollBarY->setPageStep(viewSize().height());
}

void CanvasViewController::updateScrollBarValue()
{
	d->scrollBarX->setValue(d->canvas->maxAbsoluteTranslation().x() - d->canvas->translation().x());
	d->scrollBarY->setValue(d->canvas->maxAbsoluteTranslation().y() - d->canvas->translation().y());
}

void CanvasViewController::onCanvasWillBeDeleted()
{
	setTool(0);
}

void CanvasViewController::onTransformUpdated()
{
	updateScrollBarRange();
	updateScrollBarValue();
	
	d->accurateUpdateTimer->start();
	d->viewportController->update();
}

void CanvasViewController::onRetinaModeChanged(bool retinaMode)
{
	Q_UNUSED(retinaMode);
	emit viewSizeChanged(viewSize());
	d->viewportController->update();
}

void CanvasViewController::onStrokingOrToolEditingChanged()
{
	if (!d->tool)
		return;
	
	bool strokingOrToolEditing = d->stroking || d->tool->isEditing();
	
	if (d->strokingOrToolEditing != strokingOrToolEditing)
	{
		if (strokingOrToolEditing)
		{
			d->canvas->document()->layerScene()->abortThumbnailUpdate();
			d->canvas->disableUndoRedo();
		}
		else
		{
			d->canvas->enableUndoRedo();
		}
		d->strokingOrToolEditing = strokingOrToolEditing;
	}
}

void CanvasViewController::updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects)
{
	if (!d->updateEnabled)
		return;
	
	int keyCount = keys.size();
	int rectCount = rects.size();
	
	d->viewportController->beginUpdateTile(rectCount ? rectCount : keyCount);
	
	CanvasRenderer renderer;
	renderer.setTool(d->tool);
	
	Surface surface = renderer.renderToSurface({canvas()->document()->layerScene()->rootLayer()}, keys, rects);
	
	auto documentRect = QRect(QPoint(), d->canvas->document()->size());
	
	const Pixel whitePixel(1.f);
	auto blendOp = BlendMode(BlendMode::DestinationOver).op();
	
	auto updateTile = [&](const QPoint &key, const QRect &rect)
	{
		auto relativeDocumentRect = documentRect.translated(-key * Surface::tileWidth());
		auto actualRect = rect & relativeDocumentRect;
		if (actualRect.isEmpty())
			return;
		
		auto image = surface.crop(actualRect.translated(key * Surface::tileWidth()));
		blendOp->blend(image.area(), image.bits(), whitePixel);
		
		d->viewportController->updateTile(key, image, actualRect.topLeft());
	};
	
	if (rectCount)
	{
		for (auto iter = rects.begin(); iter != rects.end(); ++iter)
			updateTile(iter.key(), iter.value());
	}
	else
	{
		for (const QPoint &key : keys)
		{
			auto rect = QRect(0, 0, Surface::tileWidth(), Surface::tileWidth());
			updateTile(key, rect);
		}
	}
	
	d->viewportController->endUpdateTile();
}

void CanvasViewController::moveWidgets()
{
	// move viewport
	{
		QRect geom(Util::mapToWindow(d->view, QPoint()), d->view->geometry().size());
		
		d->viewportController->placeViewport(d->view->window());
		d->viewportController->moveViewport(geom, d->view->isVisible());
	}
	
	// move graphics view & scroll bars
	{
		int barWidthX = d->scrollBarX->totalBarWidth();
		int barWidthY = d->scrollBarY->totalBarWidth();
		
		auto widgetRect = QRect(QPoint(), d->view->geometry().size());
		d->graphicsScene->setSceneRect(widgetRect);
		
		auto scrollBarXRect = widgetRect.adjusted(0, widgetRect.height() - barWidthY, -barWidthX, 0);
		auto scrollBarYRect = widgetRect.adjusted(widgetRect.width() - barWidthX, 0, 0, -barWidthY);
		
		d->scrollBarX->setGeometry(scrollBarXRect);
		d->scrollBarY->setGeometry(scrollBarYRect);
		
		d->graphicsView->setGeometry(widgetRect);
		d->graphicsView->setSceneRect(widgetRect);
	}
	
	// set focus on viewport
	if (d->canvas->workspace()->currentCanvas() == d->canvas)
		setFocus();
}

void CanvasViewController::setFocus()
{
	viewport()->setFocus();
}

void CanvasViewController::activate()
{
	setFocus();
	d->canvas->workspace()->setCurrentCanvas(d->canvas);
}

void CanvasViewController::onViewWidgetSizeChanged()
{
	emit viewSizeChanged(viewSize());
}

static const QString toolCursorId = "paintfield.canvas.tool";


bool CanvasViewController::eventFilter(QObject *watched, QEvent *event)
{
	bool willFilterInput = (watched == viewport());
	
	switch ((int)event->type())
	{
		case QEvent::TabletPress:
		case QEvent::MouseButtonPress:
		case EventWidgetTabletPress:
		{
			d->stroking = true;
			onStrokingOrToolEditingChanged();
			break;
		}
		case QEvent::TabletRelease:
		case QEvent::MouseButtonRelease:
		case EventWidgetTabletRelease:
		{
			d->stroking = false;
			onStrokingOrToolEditingChanged();
			break;
		}
		default:
			break;
	}
	
	switch ((int)event->type())
	{
		case QEvent::Enter:
			
			setFocus();
			appController()->cursorStack()->add(toolCursorId, d->toolCursor);
			return false;
			
		case QEvent::Leave:
			
			appController()->cursorStack()->remove(toolCursorId);
			return false;
		
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
			
			d->keyTracker->keyEvent(static_cast<QKeyEvent *>(event));
			d->eventSender->keyEvent(static_cast<QKeyEvent *>(event));
			return willFilterInput;
			
		case QEvent::MouseButtonPress:
			
			activate();
			
		case QEvent::MouseButtonRelease:
		case QEvent::MouseMove:
		
			d->navigator->mouseEvent(static_cast<QMouseEvent *>(event));
			if (event->isAccepted())
				return true;
			
		case QEvent::MouseButtonDblClick:
			
			d->eventSender->mouseEvent(static_cast<QMouseEvent *>(event));
			return false;
			
		case QEvent::TabletPress:
			
			activate();
			
		case QEvent::TabletMove:
		case QEvent::TabletRelease:
			
			d->navigator->tabletEvent(static_cast<QTabletEvent *>(event));
			if (event->isAccepted())
				return true;
			
			d->eventSender->tabletEvent(static_cast<QTabletEvent *>(event));
			return willFilterInput;
		
		case EventWidgetTabletPress:
			
			activate();
			
		case EventWidgetTabletMove:
		case EventWidgetTabletRelease:
			
			d->navigator->customTabletEvent(static_cast<WidgetTabletEvent *>(event));
			if (event->isAccepted())
				return true;
			
			d->eventSender->customTabletEvent(static_cast<WidgetTabletEvent *>(event));
			return willFilterInput;
			
		case QEvent::Wheel:
			
			d->navigator->wheelEvent(static_cast<QWheelEvent *>(event));
			return willFilterInput;
			
		default:
			break;
	}
	
	return false;
}

void CanvasViewController::onCurrentCanvasChanged(Canvas *canvas)
{
	if (d->canvas == canvas)
		setFocus();
}


} // namespace PaintField
