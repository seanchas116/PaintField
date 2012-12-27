#include <QtGui>

#include "drawutil.h"
#include "tool.h"
#include "layerrenderer.h"
#include "debug.h"
#include "canvascontroller.h"
#include "appcontroller.h"

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


class CanvasViewData
{
public:
	
	CanvasController *canvas = 0;
	QPixmap pixmap;
	ScopedQObjectPointer<Tool> tool;
	
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
	
	QPoint backupTranslation;
	double backupScale, backupRotation;
};

CanvasView::CanvasView(CanvasController *canvas, QWidget *parent) :
    NavigatableArea(parent),
    d(new CanvasViewData)
{
	d->canvas = canvas;
	d->pixmap = QPixmap(canvas->document()->size());
	
	setMouseTracking(true);
	setSceneSize(document()->size());
	
	connect(layerModel(), SIGNAL(tilesUpdated(QPointSet)), this, SLOT(updateTiles(QPointSet)));
	updateTiles(layerModel()->document()->tileKeys());
	
	connect(appController()->app(), SIGNAL(tabletActiveChanged(bool)), this, SLOT(onTabletActiveChanged(bool)));
	onTabletActiveChanged(appController()->app()->isTabletActive());
	
	d->translationKeys = appController()->keyBindingHash()["paintfield.canvas.dragTranslation"];
	d->scaleKeys = appController()->keyBindingHash()["paintfield.canvas.dragScale"];
	d->rotationKeys = appController()->keyBindingHash()["paintfield.canvas.dragRotation"];
}

CanvasView::~CanvasView()
{
	qApp->restoreOverrideCursor();
	delete d;
}

CanvasController *CanvasView::controller()
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
	d->tool.reset(tool);
	
	if (tool)
	{
		connect(tool, SIGNAL(requestUpdate(QPointSet)), this, SLOT(updateTiles(QPointSet)));
		connect(tool, SIGNAL(requestUpdate(QHash<QPoint,QRect>)), this, SLOT(updateTiles(QHash<QPoint,QRect>)));
		
		if (tool->isCustomCursorEnabled())
			d->toolCursor = QCursor(Qt::BlankCursor);
		else
		{
			connect(tool, SIGNAL(cursorChanged(QCursor)), this, SLOT(onToolCursorChanged(QCursor)));
			d->toolCursor = tool->cursor();
		}
	}
}

void CanvasView::updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects)
{
	PAINTFIELD_CALC_SCOPE_ELAPSED_TIME;
	
	CanvasRenderer renderer;
	renderer.setTool(d->tool.data());
	
	Surface surface = renderer.renderToSurface(layerModel()->rootLayer()->children(), keys, rects);
	
	QPointSet renderKeys = rects.isEmpty() ? keys : rects.keys().toSet();
	
	//PAINTFIELD_DEBUG << rects;
	
	for (const QPoint &key : renderKeys)
	{
		//Image tile = Surface::WhiteTile;
		
		QRect rect;
		
		if (!rects.isEmpty())
			rect = rects.value(key);
		else
			rect = QRect(0, 0, Surface::TileSize, Surface::TileSize);
		
		Image image(rect.size());
		image.fill(Color::fromRgbValue(1,1,1).toArgb());
		
		if (surface.contains(key))
		{
			Painter painter(&image);
			painter.drawTransformedImage(-rect.topLeft(), surface.tileForKey(key));
		}
		
		QPoint tilePos = key * Surface::TileSize;
		
		QPainter painter(&d->pixmap);
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		
		drawMLImageFast(&painter, tilePos + rect.topLeft(), image);
		
		painter.end();
		
		QRect mappedRect = transformFromScene().mapRect(QRectF(rect.translated(tilePos))).toAlignedRect();
		
		addRepaintRect(mappedRect);
		repaintDesignatedRect();
	}
}

void CanvasView::onClicked()
{
	setFocus();
	controller()->workspace()->setCurrentCanvas(controller());
}

void CanvasView::onToolCursorChanged(const QCursor &cursor)
{
	setCursor(cursor);
}

void CanvasView::onTabletActiveChanged(bool active)
{
	d->tabletActive = active;
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
	PAINTFIELD_DEBUG;
	super::enterEvent(e);
	setFocus();
	qApp->setOverrideCursor(d->toolCursor);
}

void CanvasView::leaveEvent(QEvent *e)
{
	PAINTFIELD_DEBUG;
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
	if (event->button() == Qt::RightButton)
		PAINTFIELD_DEBUG << "right clicked";
	
	if (event->button() == Qt::LeftButton)
	{
		PAINTFIELD_DEBUG << "left clicked";
		
		for (int key : d->keyTracker.pressedKeys())
			qDebug() << key;
		
		if (tryBeginDragNavigation(event->pos()))
		{
			event->accept();
			return;
		}
	}
	
	onClicked();
	
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
	{
		if (!d->tabletActive)
			d->customCursorPos = event->posF();
		
		addCustomCursorRectToRepaintRect();
		
		event->setAccepted(sendCanvasTabletEvent(event) || sendCanvasMouseEvent(event));
		
		repaintDesignatedRect();
	}
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
	
	if (event->type() == QEvent::TabletPress)
		onClicked();
	
	TabletInputData data(event->hiResGlobalPos(), event->pressure(), event->rotation(), event->tangentialPressure(), Vec2D(event->xTilt(), event->yTilt()));
	WidgetTabletEvent widgetTabletEvent(toNewEventType(event->type()), event->globalPos(), event->pos(), data, event->modifiers());
	
	customTabletEvent(&widgetTabletEvent);
	event->setAccepted(widgetTabletEvent.isAccepted());
}

void CanvasView::customTabletEvent(WidgetTabletEvent *event)
{
	if (int(event->type()) == EventWidgetTabletPress)
		onClicked();
	
	if (d->tool)
	{
		if (int(event->type()) == EventWidgetTabletMove)
		{
			d->customCursorPos = event->globalData.pos + Vec2D(event->posInt - event->globalPosInt);
			addCustomCursorRectToRepaintRect();
		}
		
		event->setAccepted(sendCanvasTabletEvent(event));
		
		repaintDesignatedRect();
	}
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
	
	CanvasMouseEvent canvasEvent(toCanvasEventType(event->type()), event->globalPos(), event->posF() * transformToScene(), event->modifiers());
	d->tool->toolEvent(&canvasEvent);
	
	return canvasEvent.isAccepted();
}

bool CanvasView::sendCanvasTabletEvent(WidgetTabletEvent *event)
{
	TabletInputData data = event->globalData;
	Vec2D globalPos = data.pos;
	data.pos += Vec2D(event->posInt - event->globalPosInt);
	data.pos *= transformToScene();
	
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
	
	TabletInputData data(mouseEvent->posF() * transformToScene(), d->mousePressure, 0, 0, Vec2D(0));
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

void CanvasView::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	
	painter.setTransform(transformFromScene());
	painter.drawPixmap(0, 0, d->pixmap);
	painter.setTransform(QTransform());
	
	if (d->tool && d->tool->isCustomCursorEnabled())
	{
		d->tool->drawCustomCursor(&painter, d->customCursorPos);
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
	d->isTranslatingByDrag = false;
	d->isScalingByDrag = false;
	d->isRotatingByDrag = false;
}

void CanvasView::beginDragTranslation(const QPoint &pos)
{
	d->isTranslatingByDrag = true;
	d->navigationOrigin = pos;
	d->backupTranslation = translation();
}

void CanvasView::continueDragTranslation(const QPoint &pos)
{
	setTranslation(d->backupTranslation + (pos - d->navigationOrigin));
}

void CanvasView::endDragTranslation()
{
	d->isTranslatingByDrag = false;
}

void CanvasView::beginDragScaling(const QPoint &pos)
{
	d->isScalingByDrag = true;
	d->navigationOrigin = pos;
	d->backupScale = scale();
	d->backupTranslation = translation();
}

void CanvasView::continueDragScaling(const QPoint &pos)
{
	auto delta = pos - d->navigationOrigin;
	
	constexpr double divisor = 100;
	
	double scaleRatio = exp2(-delta.y() / divisor);
	double scale = d->backupScale * scaleRatio;
	
	auto navigationOffset = d->navigationOrigin - viewCenter();
	
	auto translation = (d->backupTranslation - navigationOffset) * scaleRatio + navigationOffset;
	
	setScale(scale);
	setTranslation(translation);
}

void CanvasView::endDragScaling()
{
	d->isScalingByDrag = false;
}

void CanvasView::beginDragRotation(const QPoint &pos)
{
	d->isRotatingByDrag = true;
	d->navigationOrigin = pos;
	d->backupRotation = rotation();
}

void CanvasView::continueDragRotation(const QPoint &pos)
{
	QPoint originalDelta = d->navigationOrigin - viewCenter();
	QPoint delta = pos - viewCenter();
	if (delta != QPoint())
	{
		double originalRotation = -atan2(originalDelta.x(), originalDelta.y()) / M_PI * 180.0;
		double deltaRotation = -atan2(delta.x(), delta.y()) / M_PI * 180.0;
		setViewRotation(d->backupRotation + deltaRotation - originalRotation);
	}
}

void CanvasView::endDragRotation()
{
	d->isRotatingByDrag = false;
}


}
