#include <QTimer>
#include <QPointer>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include "widgets/vanishingscrollbar.h"
#include "canvasnavigator.h"
#include "canvastooleventsender.h"
#include "canvasviewport.h"
#include "canvas.h"
#include "document.h"
#include "layerscene.h"
#include "keytracker.h"
#include "appcontroller.h"
#include "tool.h"
#include "workspace.h"
#include "cursorstack.h"

#include "canvasview.h"

using namespace Malachite;

namespace PaintField {

class CanvasRenderer : public LayerRenderer
{
public:
	CanvasRenderer() : LayerRenderer() {}
	
	void setTool(Tool *tool) { _tool = tool; }
	
protected:
	
	void drawLayer(SurfacePainter *painter, const Layer *layer) override
	{
		if (_tool && _tool->layerDelegations().contains(layer))
			_tool->drawLayer(painter, layer);
		else
			LayerRenderer::drawLayer(painter, layer);
	}
	
	void renderChildren(SurfacePainter *painter, const Layer *parent) override
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
						layers << layer.pointer();
					}
					else
					{
						auto layerAt = originalLayers.at(index);
						int trueIndex = layers.indexOf(layerAt);
						layers.insert(trueIndex, layer.pointer());
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
	
	// metrics
	QSize sceneSize;
	QPoint viewCenter;
	
	// tool
	QPointer<Tool> tool;
	QCursor toolCursor;
	
	// transform
	double scale = 1, rotation = 0;
	QPoint translation;
	Affine2D transformToScene, transformFromScene;
	bool mirrored = false, retinaMode = false;
	QPoint maxAbsTranslation;
	
	// widgets
	CanvasViewport *viewport = 0;
	CanvasView *view = 0;
	QGraphicsView *graphicsView = 0;
	VanishingScrollBar *scrollBarX = 0, *scrollBarY = 0;
	
	// other objects
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
	d->sceneSize = canvas->document()->size();
	d->translation = canvas->translation();
	d->scale = canvas->scale();
	d->rotation = canvas->rotation();
	
	// setup objects
	{
		d->keyTracker = new KeyTracker(this);
		d->graphicsScene = new QGraphicsScene(this);
		d->eventSender = new CanvasToolEventSender(this);
		d->navigator = new CanvasNavigator(d->keyTracker, this);
		
		{
			auto timer = new QTimer(this);
			timer->setSingleShot(true);
			timer->setInterval(100);
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
		auto vp = new CanvasViewport();
		d->viewport = vp;
		connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)), vp, SLOT(deleteLater()));
		
		vp->setDocumentSize(d->sceneSize);
		updateTransforms();
		updateTiles(canvas->document()->tileKeys());
	}
	
	moveWidgets();
	
	// connect to canvas
	{
		connect(canvas, SIGNAL(scaleChanged(double)), this, SLOT(setScale(double)));
		connect(canvas, SIGNAL(rotationChanged(double)), this, SLOT(setRotation(double)));
		connect(canvas, SIGNAL(translationChanged(QPoint)), this, SLOT(setTranslation(QPoint)));
		connect(canvas, SIGNAL(mirroredChanged(bool)), this, SLOT(setMirrored(bool)));
		connect(canvas, SIGNAL(retinaModeChanged(bool)), this, SLOT(setRetinaMode(bool)));
		
		setScale(canvas->scale());
		setRotation(canvas->rotation());
		setTranslation(canvas->translation());
		
		connect(canvas, SIGNAL(toolChanged(Tool*)), this, SLOT(setTool(Tool*)));
		setTool(canvas->tool());
		
		canvas->setViewController(this);
		
		connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)), this, SLOT(onCanvasWillBeDeleted()));
	}
	
	connect(canvas->document()->layerScene(), SIGNAL(tilesUpdated(QPointSet)), this, SLOT(updateTiles(QPointSet)));
}

CanvasViewController::~CanvasViewController()
{
	delete d;
}

CanvasView *CanvasViewController::view()
{
	return d->view;
}

Malachite::Affine2D CanvasViewController::transformToScene() const
{
	return d->transformToScene;
}

Malachite::Affine2D CanvasViewController::transformFromScene() const
{
	return d->transformFromScene;
}

QPoint CanvasViewController::viewCenter() const
{
	return d->viewCenter;
}

bool CanvasViewController::isUpdateTilesEnabled() const
{
	return d->updateEnabled;
}

Canvas *CanvasViewController::canvas()
{
	return d->canvas;
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
	
	d->viewport->updateWholeAccurately();
}

void CanvasViewController::setScale(double value)
{
	d->scale = value;
	updateTransforms();
}

void CanvasViewController::setRotation(double value)
{
	d->rotation = value;
	updateTransforms();
}

void CanvasViewController::setTranslation(const QPoint &value)
{
	d->translation = value;
	updateTransforms();
}

void CanvasViewController::setMirrored(bool value)
{
	d->mirrored = value;
	updateTransforms();
}

void CanvasViewController::setRetinaMode(bool value)
{
	d->retinaMode = value;
	updateTransforms();
}

void CanvasViewController::onClicked()
{
	d->view->setFocus();
	d->canvas->workspace()->setCurrentCanvas(d->canvas);
}

void CanvasViewController::onScrollBarXChanged(int x)
{
	d->canvas->setTranslationX(d->maxAbsTranslation.x() - x);
}

void CanvasViewController::onScrollBarYChanged(int y)
{
	d->canvas->setTranslationY(d->maxAbsTranslation.y() - y);
}

void CanvasViewController::updateScrollBarRange()
{
	int radius = ceil(hypot(d->sceneSize.width(), d->sceneSize.height()) * d->scale * 0.5);
	
	d->maxAbsTranslation = QPoint(radius + d->view->width(), radius + d->view->height());
	
	d->scrollBarX->setRange(0, 2 * d->maxAbsTranslation.x());
	d->scrollBarY->setRange(0, 2 * d->maxAbsTranslation.y());
	
	d->scrollBarX->setPageStep(d->view->width());
	d->scrollBarY->setPageStep(d->view->height());
}

void CanvasViewController::updateScrollBarValue()
{
	d->scrollBarX->setValue(d->maxAbsTranslation.x() - d->translation.x());
	d->scrollBarY->setValue(d->maxAbsTranslation.y() - d->translation.y());
}

void CanvasViewController::onCanvasWillBeDeleted()
{
	setTool(0);
}

void CanvasViewController::updateTransforms()
{
	QPoint sceneOffset = QPoint(d->sceneSize.width(), d->sceneSize.height()) / 2;
	QPoint viewOffset = viewCenter() + d->translation;
	
	double scale = d->retinaMode ? d->scale * 0.5 : d->scale;
	
	auto transform = Affine2D::fromTranslation(Vec2D(viewOffset)) *
	                 Affine2D::fromRotationDegrees(d->rotation) *
	                 Affine2D::fromScale(scale) *
	                 Affine2D::fromTranslation(Vec2D(-sceneOffset));
	
	if (d->mirrored)
		transform = transform * Affine2D(-1, 0, 0, 1, d->sceneSize.width(), 0);
	
	d->transformFromScene = transform;
	d->transformToScene = transform.inverted();
	d->viewport->setTransform(transform, d->translation, scale, d->rotation, d->retinaMode);
	
	updateScrollBarRange();
	updateScrollBarValue();
	
	emit transformUpdated();
	
	d->accurateUpdateTimer->start();
	
	d->viewport->update();
}

void CanvasViewController::updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects)
{
	if (!d->updateEnabled)
		return;
	
	d->viewport->beforeUpdateTile(CanvasViewport::PartialAccurateUpdate);
	
	CanvasRenderer renderer;
	renderer.setTool(d->tool);
	
	Surface surface = renderer.renderToSurface(canvas()->document()->layerScene()->rootLayer().pointer(), keys, rects);
	
	static const Pixel whitePixel = Color::fromRgbValue(1,1,1).toPixel();
	
	auto updateTile = [this, &surface](const QPoint &key, const QRect &rect)
	{
		Image image(rect.size());
		image.fill(whitePixel);
		
		if (surface.contains(key))
		{
			Painter painter(&image);
			painter.drawPreTransformedImage(-rect.topLeft(), surface.tile(key));
		}
		
		d->viewport->updateTile(key, image, rect.topLeft());
	};
	
	if (rects.isEmpty())
	{
		for (const QPoint &key : keys)
		{
			auto rect = QRect(0, 0, Surface::tileWidth(), Surface::tileWidth());
			updateTile(key, rect);
		}
	}
	else
	{
		for (auto iter = rects.begin(); iter != rects.end(); ++iter)
		{
			updateTile(iter.key(), iter.value());
		}
	}
	
	d->viewport->afterUpdateTile();
}

void CanvasViewController::moveWidgets()
{
	// move viewport
	{
		QRect geom(Util::mapToWindow(d->view, QPoint()), d->view->geometry().size());
		
		d->viewport->setParent(d->view->window());
		d->viewport->setGeometry(geom);
		d->viewport->show();
		d->viewport->lower();
		d->viewport->setEnabled(d->view->isEnabled());
		d->viewport->setVisible(d->view->isVisible());
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
	
	updateTransforms();
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
				
				d->viewCenter = QPoint(d->view->width() / 2, d->view->height() / 2);
				
			case QEvent::ParentChange:
			case QEvent::EnabledChange:
			case QEvent::Show:
			case QEvent::Hide:
				
				moveWidgets();
				return false;
				
			case QEvent::FocusIn:
				
				d->viewport->updateWholeAccurately();
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
