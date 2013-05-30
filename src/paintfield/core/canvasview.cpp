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

class CanvasRenderer : public LayerRenderer
{
public:
	CanvasRenderer() : LayerRenderer() {}
	
	void setTool(Tool *tool) { _tool = tool; }
	
protected:
	
	void drawLayer(SurfacePainter *painter, const LayerConstPtr &layer) override
	{
		if (_tool && _tool->layerDelegations().contains(layer))
			_tool->drawLayer(painter, layer);
		else
			LayerRenderer::drawLayer(painter, layer);
	}
	
	void renderChildren(SurfacePainter *painter, const LayerConstPtr &parent) override
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
	CanvasViewportController *viewportContoller = 0;
	KeyTracker *keyTracker = 0;
	QGraphicsScene *graphicsScene = 0;
	QTimer *accurateUpdateTimer = 0;
	CanvasToolEventSender *eventSender = 0;
	CanvasNavigator *navigator = 0;
	
	// other
	bool updateEnabled = true;
	bool graphicsViewDraggingItem = false;
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
		d->view = new CanvasView(canvas);
		d->view->setMouseTracking(true);
		d->view->installEventFilter(this);
		d->view->setFocusPolicy(Qt::ClickFocus);
		connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)), d->view, SLOT(deleteLater()));
		
		d->graphicsView = new QGraphicsView(d->view);
		d->graphicsView->viewport()->installEventFilter(this);
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
			d->scrollBarX = new VanishingScrollBar(Qt::Horizontal, d->view);
			d->scrollBarY = new VanishingScrollBar(Qt::Vertical, d->view);
			
			connect(d->scrollBarX, SIGNAL(valueChanged(int)), this, SLOT(onScrollBarXChanged(int)));
			connect(d->scrollBarY, SIGNAL(valueChanged(int)), this, SLOT(onScrollBarYChanged(int)));
		}
	}
	
	// setup viewport
	{
		auto vp = new CanvasViewportController(this);
		d->viewportContoller = vp;
		connect(vp, SIGNAL(viewSizeChanged(QSize)), canvas, SLOT(setViewSize(QSize)));
		connect(canvas, SIGNAL(transformChanged(Malachite::Affine2D,Malachite::Affine2D)), vp, SLOT(setTransform(Malachite::Affine2D,Malachite::Affine2D)));
		connect(canvas, SIGNAL(retinaModeChanged(bool)), vp, SLOT(setRetinaMode(bool)));
		vp->setTransform(canvas->transformToScene(), canvas->transformToView());
		vp->setRetinaMode(canvas->isRetinaMode());
		vp->setDocumentSize(canvas->document()->size());
	}
	
	moveWidgets();
	
	// connect to canvas
	{
		connect(canvas, SIGNAL(transformChanged(Malachite::Affine2D,Malachite::Affine2D)), this, SLOT(onTransformUpdated(Malachite::Affine2D,Malachite::Affine2D)));
		onTransformUpdated(canvas->transformToScene(), canvas->transformToView());
		
		connect(canvas, SIGNAL(toolChanged(Tool*)), this, SLOT(setTool(Tool*)));
		setTool(canvas->tool());
		
		canvas->setViewController(this);
		
		connect(this, SIGNAL(viewSizeChanged(QSize)), canvas, SLOT(setViewSize(QSize)));
		canvas->setViewSize(viewSize());
		connect(canvas, SIGNAL(retinaModeChanged(bool)), this, SLOT(onRetinaModeChanged(bool)));
		
		connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)), this, SLOT(onCanvasWillBeDeleted()));
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
		
		d->toolCursor = tool->cursor();
		
		if (tool->graphicsItem())
		{
			d->graphicsScene->addItem(tool->graphicsItem());
			d->graphicsView->setVisible(true);
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
	
	d->viewportContoller->update();
}

void CanvasViewController::onClicked()
{
	d->view->setFocus();
	d->canvas->workspace()->setCurrentCanvas(d->canvas);
}

void CanvasViewController::onScrollBarXChanged(int x)
{
	d->canvas->setTranslationX(d->canvas->maxAbsoluteTranslation().x() - x);
}

void CanvasViewController::onScrollBarYChanged(int y)
{
	d->canvas->setTranslationY(d->canvas->maxAbsoluteTranslation().y() - y);
}

void CanvasViewController::updateScrollBarRange()
{
	auto maxAbsTranslation = d->canvas->maxAbsoluteTranslation();
	auto viewSize = d->view->size();
	
	d->scrollBarX->setRange(0, 2 * maxAbsTranslation.x());
	d->scrollBarY->setRange(0, 2 * maxAbsTranslation.y());
	
	d->scrollBarX->setPageStep(viewSize.width());
	d->scrollBarY->setPageStep(viewSize.height());
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

void CanvasViewController::onTransformUpdated(const Affine2D &transformToScene, const Affine2D &transformToView)
{
	Q_UNUSED(transformToScene);
	Q_UNUSED(transformToView);
	
	updateScrollBarRange();
	updateScrollBarValue();
	
	d->accurateUpdateTimer->start();
	d->viewportContoller->update();
}

void CanvasViewController::onRetinaModeChanged(bool retinaMode)
{
	Q_UNUSED(retinaMode);
	emit viewSizeChanged(viewSize());
	d->viewportContoller->update();
}

void CanvasViewController::updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects)
{
	if (!d->updateEnabled)
		return;
	
	int keyCount = keys.size();
	int rectCount = rects.size();
	
	d->viewportContoller->beginUpdateTile(rectCount ? rectCount : keyCount);
	
	CanvasRenderer renderer;
	renderer.setTool(d->tool);
	
	Surface surface = renderer.renderToSurface({canvas()->document()->layerScene()->rootLayer()}, keys, rects);
	
	static const Pixel whitePixel = Color::fromRgbValue(1,1,1).toPixel();
	auto documentRect = QRect(QPoint(), d->canvas->document()->size());
	
	auto updateTile = [&](const QPoint &key, const QRect &rect)
	{
		auto relativeDocumentRect = documentRect.translated(-key * Surface::tileWidth());
		auto actualRect = rect & relativeDocumentRect;
		if (actualRect.isEmpty())
			return;
		
		Image image(actualRect.size());
		image.fill(whitePixel);
		
		if (surface.contains(key))
		{
			Painter painter(&image);
			painter.drawPreTransformedImage(-actualRect.topLeft(), surface.tile(key));
		}
		
		d->viewportContoller->updateTile(key, image, actualRect.topLeft());
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
	
	d->viewportContoller->endUpdateTile();
}

void CanvasViewController::moveWidgets()
{
	// move viewport
	{
		QRect geom(Util::mapToWindow(d->view, QPoint()), d->view->geometry().size());
		
		d->viewportContoller->placeViewport(d->view->window());
		d->viewportContoller->moveViewport(geom, d->view->isVisible());
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
}

static const QString toolCursorId = "paintfield.canvas.tool";


bool CanvasViewController::eventFilter(QObject *watched, QEvent *event)
{
	bool willFilterInput = (watched == d->view);
	
	switch ((int)event->type())
	{
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
			
			d->keyTracker->keyEvent(static_cast<QKeyEvent *>(event));
			d->eventSender->keyEvent(static_cast<QKeyEvent *>(event));
			return willFilterInput;
			
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		case QEvent::MouseMove:
		
			d->navigator->mouseEvent(static_cast<QMouseEvent *>(event));
			if (event->isAccepted())
				return true;
			
		case QEvent::MouseButtonDblClick:
			
			d->eventSender->mouseEvent(static_cast<QMouseEvent *>(event));
			return false;
			
		case QEvent::TabletMove:
		case QEvent::TabletRelease:
		case QEvent::TabletPress:
			
			d->navigator->tabletEvent(static_cast<QTabletEvent *>(event));
			if (event->isAccepted())
				return true;
			
			d->eventSender->tabletEvent(static_cast<QTabletEvent *>(event));
			return willFilterInput;
			
		case EventWidgetTabletMove:
		case EventWidgetTabletPress:
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
	
	if (watched == d->view)
	{
		switch (event->type())
		{
			case QEvent::Resize:
				
				emit viewSizeChanged(viewSize());
				
			case QEvent::ParentChange:
			case QEvent::EnabledChange:
			case QEvent::Show:
			case QEvent::Hide:
				
				moveWidgets();
				return false;
				
			case QEvent::FocusIn:
				
				return false;
				
			case QEvent::Enter:
				
				d->view->setFocus();
				appController()->cursorStack()->add(toolCursorId, d->toolCursor);
				return false;
				
			case QEvent::Leave:
				
				appController()->cursorStack()->remove(toolCursorId);
				return false;
				
			default:
				break;
		}
	}
	
	return false;
}



} // namespace PaintField
