#include <QtGui>

#include "application.h"
#include "settingsmanager.h"
#include "smartpointer.h"
#include "drawutil.h"
#include "tool.h"
#include "layerrenderer.h"
#include "canvas.h"
#include "appcontroller.h"
#include "workspace.h"
#include "keytracker.h"
#include "widgets/vanishingscrollbar.h"

#include "abstractcanvasviewportcontroller.h"
#include "canvasviewportsoftware.h"

#include "canvasview.h"

namespace PaintField
{

using namespace Malachite;


class CanvasRenderer : public LayerRenderer
{
public:
	CanvasRenderer() : LayerRenderer() {}
	
	void setTool(Tool *tool) { _tool = tool; }
	
protected:
	
	void drawLayer(SurfacePainter *painter, const Layer *layer)
	{
		if (_tool && _tool->customDrawLayers().contains(layer))
			_tool->drawLayer(painter, layer);
		else
			LayerRenderer::drawLayer(painter, layer);
	}
	
private:
	
	Tool *_tool = 0;
};

struct Navigation
{
	double scale = 1, rotation = 0;
	QPoint translation;
};

class CanvasView::Data
{
public:
	
	Canvas *canvas = 0;
	Tool *tool = 0;
	
	double mousePressure = 0;
	QRect repaintRect;
	QRect prevCustomCursorRect;
	Malachite::Vec2D customCursorPos;
	bool tabletActive = false;
	
	KeyTracker keyTracker;
	
	QCursor toolCursor;
	
	QKeySequence scaleKeys, rotationKeys, translationKeys;
	
	bool isScalingByDrag = false, isRotatingByDrag = false, isTranslatingByDrag = false;
	QPoint navigationOrigin;
	
	Navigation nav, backupNav, memorizedNav;
	bool mirror = false;
	
	QSize sceneSize;
	QPoint viewCenter;
	
	Affine2D transformToScene, transformFromScene;
	
	CanvasViewportInterface *viewport;
	QWidget *viewportWidget;
	
	QPoint maxAbsTranslation;
};

CanvasView::CanvasView(Canvas *canvas, QWidget *parent) :
    QWidget(parent),
    d(new Data)
{
	d->canvas = canvas;
	d->sceneSize = canvas->document()->size();
	
	
	// setup viewport
	{
		auto viewport = new CanvasViewportSoftware(this);
		d->viewport = viewport;
		d->viewportWidget = viewport;
		
		if (viewport->isReady())
			onViewportReady();
		else
			connect(viewport, SIGNAL(ready()), this, SLOT(onViewportReady()));
		
		auto layout = new QVBoxLayout;
		layout->addWidget(viewport);
		viewport->setMouseTracking(true);
		layout->setContentsMargins(0, 0, 0 ,0);
		setLayout(layout);
		
		d->viewCenter = QPoint(width() / 2, height() / 2);
	}
	
	{
		connect(d->viewportWidget, SIGNAL(scrollBarXChanged(int)), this, SLOT(onScrollBarXChanged(int)));
		connect(d->viewportWidget, SIGNAL(scrollBarYChanged(int)), this, SLOT(onScrollBarYChanged(int)));
	}
	
	// connect to canvas
	{
		connect(canvas, SIGNAL(scaleChanged(double)), this, SLOT(setScale(double)));
		connect(canvas, SIGNAL(rotationChanged(double)), this, SLOT(setRotation(double)));
		connect(canvas, SIGNAL(translationChanged(QPoint)), this, SLOT(setTranslation(QPoint)));
		
		setScale(canvas->scale());
		setRotation(canvas->rotation());
		setTranslation(canvas->translation());
		
		connect(canvas, SIGNAL(toolChanged(Tool*)), this, SLOT(setTool(Tool*)));
		setTool(canvas->tool());
		
		connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)), this, SLOT(deleteLater()));
		
		canvas->setView(this);
	}
	
	connect(layerModel(), SIGNAL(tilesUpdated(QPointSet)),
	        this, SLOT(updateTiles(QPointSet)));
	
	connect(appController()->app(), SIGNAL(tabletActiveChanged(bool)),
	        this, SLOT(onTabletActiveChanged(bool)));
	onTabletActiveChanged(appController()->app()->isTabletActive());
	
	// setup key bindings
	{
		auto keyBindingHash = appController()->settingsManager()->settings()[".key-bindings"].toMap();
		
		d->translationKeys = keyBindingHash["paintfield.canvas.dragTranslation"].toString();
		d->scaleKeys = keyBindingHash["paintfield.canvas.dragScale"].toString();
		d->rotationKeys = keyBindingHash["paintfield.canvas.dragRotation"].toString();
	}
	
	// set widget properties
	{
		setMouseTracking(true);
	}
}

CanvasView::~CanvasView()
{
	while (qApp->overrideCursor())
		qApp->restoreOverrideCursor();
	
	delete d;
}


QPoint CanvasView::viewCenter() const
{
	return d->viewCenter;
}

void CanvasView::setScale(double value)
{
	d->nav.scale = value;
	updateTransforms();
}

void CanvasView::setRotation(double value)
{
	d->nav.rotation = value;
	updateTransforms();
}

void CanvasView::setTranslation(const QPoint &value)
{
	d->nav.translation = value;
	updateTransforms();
}

void CanvasView::memorizeNavigation()
{
	d->memorizedNav = d->nav;
}

void CanvasView::restoreNavigation()
{
	d->nav = d->memorizedNav;
}

Affine2D CanvasView::transformToScene() const
{
	return d->transformToScene;
}

Affine2D CanvasView::transformFromScene() const
{
	return d->transformFromScene;
}

void CanvasView::updateTransforms()
{
	QPoint sceneOffset = QPoint(d->sceneSize.width(), d->sceneSize.height()) / 2;
	QPoint viewOffset = viewCenter() + d->nav.translation;
	
	auto transform = Affine2D::fromTranslation(Vec2D(viewOffset)) *
	                 Affine2D::fromRotationDegrees(d->nav.rotation) *
	                 Affine2D::fromScale(d->nav.scale) *
	                 Affine2D::fromTranslation(Vec2D(-sceneOffset));
	
	d->transformFromScene = transform;
	d->transformToScene = transform.inverted();
	d->viewport->setTransform(transform, d->nav.translation != QPoint(), d->nav.scale != 1.0, d->nav.rotation != 0);
	
	updateScrollBarRange();
	updateScrollBarValue();
	
	update();
}

Canvas *CanvasView::canvas()
{
	return d->canvas;
}

Document *CanvasView::document()
{
	return d->canvas->document();
}

LayerModel *CanvasView::layerModel()
{
	return d->canvas->layerModel();
}

void CanvasView::setTool(Tool *tool)
{
	if (tool)
	{
		d->tool = tool;
		
		connect(tool, SIGNAL(requestUpdate(QPointSet)), this, SLOT(updateTiles(QPointSet)));
		connect(tool, SIGNAL(requestUpdate(QHash<QPoint,QRect>)), this, SLOT(updateTiles(QHash<QPoint,QRect>)));
		
		if (tool->isCustomCursorEnabled())
		{
			d->toolCursor = QCursor(Qt::BlankCursor);
			setCursor(QCursor(Qt::BlankCursor));
		}
		else
		{
			connect(tool, SIGNAL(cursorChanged(QCursor)), this, SLOT(onToolCursorChanged(QCursor)));
			d->toolCursor = tool->cursor();
			setCursor(tool->cursor());
		}
	}
}

void CanvasView::updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects)
{
	d->viewport->beforeUpdateTile();
	
	CanvasRenderer renderer;
	renderer.setTool(d->tool);
	
	Surface surface = renderer.renderToSurface(layerModel()->rootLayer()->children(), keys, rects);
	
	QPointSet renderKeys = rects.isEmpty() ? keys : rects.keys().toSet();

	for (const QPoint &key : renderKeys)
	{
		QRect rect;
		
		if (!rects.isEmpty())
		{
			rect = rects.value(key);
			if (rect.isEmpty())
				continue;
		}
		else
		{
			rect = QRect(0, 0, Surface::tileWidth(), Surface::tileWidth());
		}
		
		Image image(rect.size());
		image.fill(Color::fromRgbValue(1,1,1).toPixel());
		
		if (surface.contains(key))
		{
			Painter painter(&image);
			painter.drawPreTransformedImage(-rect.topLeft(), surface.tile(key));
		}
		
		d->viewport->updateTile(key, image, rect.topLeft());
	}
	
	d->viewport->afterUpdateTile();
}

void CanvasView::onClicked()
{
	setFocus();
	canvas()->workspace()->setCurrentCanvas(canvas());
}

void CanvasView::onToolCursorChanged(const QCursor &cursor)
{
	setCursor(cursor);
}

void CanvasView::onTabletActiveChanged(bool active)
{
	d->tabletActive = active;
}

void CanvasView::onViewportReady()
{
	d->viewport->setDocumentSize(d->sceneSize);
	updateTransforms();
	updateTiles(layerModel()->document()->tileKeys());
}

void CanvasView::onScrollBarXChanged(int value)
{
	d->canvas->setTranslationX(d->maxAbsTranslation.x() - value);
}

void CanvasView::onScrollBarYChanged(int value)
{
	d->canvas->setTranslationY(d->maxAbsTranslation.y() - value);
}

void CanvasView::updateScrollBarRange()
{
	int radius = ceil(hypot(d->sceneSize.width(), d->sceneSize.height()) * d->nav.scale * 0.5);
	
	d->maxAbsTranslation = QPoint(radius + this->width(), radius + this->height());
	
	d->viewport->setScrollBarRange(Qt::Horizontal, 0, 2 * d->maxAbsTranslation.x());
	d->viewport->setScrollBarRange(Qt::Vertical, 0, 2 * d->maxAbsTranslation.y());
	
	d->viewport->setScrollBarPageStep(Qt::Horizontal, this->width());
	d->viewport->setScrollBarPageStep(Qt::Vertical, this->height());
}

void CanvasView::updateScrollBarValue()
{
	d->viewport->setScrollBarValue(Qt::Horizontal, d->maxAbsTranslation.x() - d->nav.translation.x());
	d->viewport->setScrollBarValue(Qt::Vertical, d->maxAbsTranslation.y() - d->nav.translation.y());
}

void CanvasView::keyPressEvent(QKeyEvent *event)
{
	if (d->tool)
		d->tool->toolEvent(event);
	
	PAINTFIELD_DEBUG << "pressed:" << event->key() << "modifiers" << event->modifiers();
	d->keyTracker.keyPressed(event->key());
}

void CanvasView::keyReleaseEvent(QKeyEvent *event)
{
	if (d->tool)
		d->tool->toolEvent(event);
	
	PAINTFIELD_DEBUG << "released:" << event->key() << "modifiers" << event->modifiers();
	d->keyTracker.keyReleased(event->key());
}

void CanvasView::enterEvent(QEvent *e)
{
	super::enterEvent(e);
	setFocus();
	qApp->setOverrideCursor(d->toolCursor);
}

void CanvasView::leaveEvent(QEvent *e)
{
	super::leaveEvent(e);
	
	while (qApp->overrideCursor())
		qApp->restoreOverrideCursor();
}

void CanvasView::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (d->tool)
		event->setAccepted(sendCanvasMouseEvent(event));
}

void CanvasView::mousePressEvent(QMouseEvent *event)
{
	onClicked();
	
	if (event->button() == Qt::LeftButton)
	{
		if (tryBeginDragNavigation(event->pos()))
		{
			event->accept();
			return;
		}
	}
	
	if (d->tool)
		event->setAccepted(sendCanvasTabletEvent(event) || sendCanvasMouseEvent(event));
}

void CanvasView::mouseMoveEvent(QMouseEvent *event)
{
	if (continueDragNavigation(event->pos()))
	{
		event->accept();
		return;
	}
	
	if (d->tool)
		event->setAccepted(sendCanvasTabletEvent(event) || sendCanvasMouseEvent(event));
}

void CanvasView::mouseReleaseEvent(QMouseEvent *event)
{
	endDragNavigation();
	
	if (d->tool)
		event->setAccepted(sendCanvasTabletEvent(event) || sendCanvasMouseEvent(event));
}

void CanvasView::tabletEvent(QTabletEvent *event)
{
	auto toNewEventType = [](QEvent::Type type)
	{
		switch (type)
		{
			default:
			case QEvent::TabletMove:
				return PaintField::EventWidgetTabletMove;
			case QEvent::TabletPress:
				return PaintField::EventWidgetTabletPress;
			case QEvent::TabletRelease:
				return PaintField::EventWidgetTabletRelease;
		}
	};
	
	switch (event->type())
	{
		case QEvent::TabletPress:
			
			onClicked();
			if (tryBeginDragNavigation(event->pos()))
			{
				event->accept();
				return;
			}
			break;
			
		case QEvent::TabletMove:
			
			if (continueDragNavigation(event->pos()))
			{
				event->accept();
				return;
			}
			break;
			
		case QEvent::TabletRelease:
			
			endDragNavigation();
			break;
			
		default:
			break;
	}
	
	TabletInputData data(event->hiResGlobalPos(), event->pressure(), event->rotation(), event->tangentialPressure(), Vec2D(event->xTilt(), event->yTilt()));
	WidgetTabletEvent widgetTabletEvent(toNewEventType(event->type()), event->globalPos(), event->pos(), data, event->modifiers());
	
	customTabletEvent(&widgetTabletEvent);
	event->setAccepted(widgetTabletEvent.isAccepted());
}

void CanvasView::customTabletEvent(WidgetTabletEvent *event)
{
	
	switch (int(event->type()))
	{
		case EventWidgetTabletPress:
			
			onClicked();
			if (tryBeginDragNavigation(event->posInt))
			{
				event->accept();
				return;
			}
			break;
			
		case EventWidgetTabletMove:
			
			if (continueDragNavigation(event->posInt))
			{
				event->accept();
				return;
			}
			break;
			
		case EventWidgetTabletRelease:
			
			endDragNavigation();
			break;
			
		default:
			break;
	}
	
	if (d->tool)
	{
		event->setAccepted(sendCanvasTabletEvent(event));
	}
}

void CanvasView::wheelEvent(QWheelEvent *event)
{
	QPoint translation = d->canvas->translation();
	
	if (event->orientation() == Qt::Horizontal)
		translation += QPoint(event->delta(), 0);
	else
		translation += QPoint(0, event->delta());
	
	d->canvas->setTranslation(translation);
}

void CanvasView::resizeEvent(QResizeEvent *)
{
	d->viewCenter = QPoint(width() / 2, height() / 2);
	updateTransforms();
}

bool CanvasView::event(QEvent *event)
{
	switch ((int)event->type())
	{
		case EventWidgetTabletMove:
		case EventWidgetTabletPress:
		case EventWidgetTabletRelease:
			customTabletEvent(static_cast<WidgetTabletEvent *>(event));
			return event->isAccepted();
		default:
			return QWidget::event(event);
	}
}

bool CanvasView::sendCanvasMouseEvent(QMouseEvent *event)
{
	auto toCanvasEventType = [](QEvent::Type type)
	{
		switch (type)
		{
			default:
			case QEvent::MouseMove:
				return EventCanvasMouseMove;
			case QEvent::MouseButtonPress:
				return EventCanvasMousePress;
			case QEvent::MouseButtonRelease:
				return EventCanvasMouseRelease;
			case QEvent::MouseButtonDblClick:
				return EventCanvasMouseDoubleClick;
		}
	};
	
	CanvasMouseEvent canvasEvent(toCanvasEventType(event->type()), event->globalPos(), transformToScene() * event->posF(), event->modifiers());
	d->tool->toolEvent(&canvasEvent);
	
	return canvasEvent.isAccepted();
}

bool CanvasView::sendCanvasTabletEvent(WidgetTabletEvent *event)
{
	TabletInputData data = event->globalData;
	Vec2D globalPos = data.pos;
	data.pos += Vec2D(event->posInt - event->globalPosInt);
	data.pos = transformToScene() * data.pos;
	
	auto toCanvasEventType = [](int type)
	{
		switch (type)
		{
			default:
			case EventWidgetTabletMove:
				return EventCanvasTabletMove;
			case EventWidgetTabletPress:
				return EventCanvasTabletPress;
			case EventWidgetTabletRelease:
				return EventCanvasTabletRelease;
		}
	};
	
	CanvasTabletEvent canvasEvent(toCanvasEventType(event->type()), globalPos, event->globalPosInt, data, event->modifiers());
	d->tool->toolEvent(&canvasEvent);
	
	return canvasEvent.isAccepted();
}

bool CanvasView::sendCanvasTabletEvent(QMouseEvent *mouseEvent)
{
	auto toCanvasEventType = [](QEvent::Type type)
	{
		switch (type)
		{
			default:
			case QEvent::MouseMove:
				return EventCanvasTabletMove;
			case QEvent::MouseButtonPress:
				return EventCanvasTabletPress;
			case QEvent::MouseButtonRelease:
				return EventCanvasTabletRelease;
		}
	};
	
	int type = toCanvasEventType(mouseEvent->type());
	
	if (type == EventCanvasTabletPress)
		d->mousePressure = 1.0;
	if (type == EventCanvasTabletRelease)
		d->mousePressure = 0.0;
	
	TabletInputData data(transformToScene() * mouseEvent->posF(), d->mousePressure, 0, 0, Vec2D(0));
	CanvasTabletEvent tabletEvent(type, mouseEvent->globalPos(), mouseEvent->globalPos(), data, mouseEvent->modifiers());
	d->tool->toolEvent(&tabletEvent);
	return tabletEvent.isAccepted();
}

void CanvasView::addCustomCursorRectToRepaintRect()
{
	if (d->tool->isCustomCursorEnabled())
	{
		addRepaintRect(d->prevCustomCursorRect);
		
		auto rect = d->tool->customCursorRect(d->customCursorPos);
		d->prevCustomCursorRect = rect;
		
		addRepaintRect(rect);
	}
}

void CanvasView::addRepaintRect(const QRect &rect)
{
	d->repaintRect |= rect;
}

void CanvasView::repaintDesignatedRect()
{
	if (d->repaintRect.isValid())
	{
		//PAINTFIELD_DEBUG << "repainting" << _repaintRect;
		repaint(d->repaintRect);
		d->repaintRect = QRect();
	}
}

bool CanvasView::tryBeginDragNavigation(const QPoint &pos)
{
	if (d->keyTracker.match(d->scaleKeys))
	{
		beginDragScaling(pos);
		return true;
	}
	if (d->keyTracker.match(d->rotationKeys))
	{
		beginDragRotation(pos);
		return true;
	}
	 if (d->keyTracker.match(d->translationKeys))
	{
		beginDragTranslation(pos);
		return true;
	}
	
	return false;
}

bool CanvasView::continueDragNavigation(const QPoint &pos)
{
	if (d->isTranslatingByDrag)
	{
		continueDragTranslation(pos);
		return true;
	}
	if (d->isScalingByDrag)
	{
		continueDragScaling(pos);
		return true;
	}
	if (d->isRotatingByDrag)
	{
		continueDragRotation(pos);
		return true;
	}
	
	return false;
}

void CanvasView::endDragNavigation()
{
	endDragTranslation();
	endDragScaling();
	endDragRotation();
}

void CanvasView::beginDragTranslation(const QPoint &pos)
{
	qApp->setOverrideCursor(Qt::ClosedHandCursor);
	
	d->isTranslatingByDrag = true;
	d->navigationOrigin = pos;
	d->backupNav = d->nav;
}

void CanvasView::continueDragTranslation(const QPoint &pos)
{
	d->canvas->setTranslation(d->backupNav.translation + (pos - d->navigationOrigin));
}

void CanvasView::endDragTranslation()
{
	qApp->restoreOverrideCursor();
	d->isTranslatingByDrag = false;
}

void CanvasView::beginDragScaling(const QPoint &pos)
{
	qApp->setOverrideCursor(Qt::SizeVerCursor);
	
	d->isScalingByDrag = true;
	d->navigationOrigin = pos;
	d->backupNav = d->nav;
}

void CanvasView::continueDragScaling(const QPoint &pos)
{
	auto delta = pos - d->navigationOrigin;
	
	constexpr double divisor = 100;
	
	double scaleRatio = exp2(-delta.y() / divisor);
	double scale = d->backupNav.scale * scaleRatio;
	
	auto navigationOffset = d->navigationOrigin - viewCenter();
	
	auto translation = (d->backupNav.translation - navigationOffset) * scaleRatio + navigationOffset;
	
	d->canvas->setScale(scale);
	d->canvas->setTranslation(translation);
}

void CanvasView::endDragScaling()
{
	qApp->restoreOverrideCursor();
	d->isScalingByDrag = false;
}

void CanvasView::beginDragRotation(const QPoint &pos)
{
	qApp->setOverrideCursor(Qt::ClosedHandCursor);
	
	d->isRotatingByDrag = true;
	d->navigationOrigin = pos;
	d->backupNav = d->nav;
}

void CanvasView::continueDragRotation(const QPoint &pos)
{
	auto originalDelta = d->navigationOrigin - viewCenter();
	auto delta = pos - viewCenter();
	if (delta != QPoint())
	{
		auto originalRotation = -atan2(originalDelta.x(), originalDelta.y()) / M_PI * 180.0;
		auto deltaRotation = -atan2(delta.x(), delta.y()) / M_PI * 180.0;
		
		auto navigationOffset = d->navigationOrigin - viewCenter();
		
		auto rotation = d->backupNav.rotation + deltaRotation - originalRotation;
		
		QTransform transform;
		transform.rotate(rotation);
		
		auto translation = (d->backupNav.translation - navigationOffset) * transform + navigationOffset;
		
		d->canvas->setRotation(rotation);
		d->canvas->setTranslation(translation);
	}
}

void CanvasView::endDragRotation()
{
	qApp->restoreOverrideCursor();
	
	d->isRotatingByDrag = false;
}


}
