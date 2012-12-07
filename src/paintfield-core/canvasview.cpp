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


CanvasView::CanvasView(CanvasController *canvas, QWidget *parent) :
    QWidget(parent),
    _canvas(canvas),
    _document(canvas->document()),
    _pixmap(_document->size())
{
	setMouseTracking(true);
	
	connect(layerModel(), SIGNAL(tilesUpdated(QPointSet)), this, SLOT(updateTiles(QPointSet)));
	updateTiles(layerModel()->document()->tileKeys());
	updateTransforms();
	
	int width = qMax(_pixmap.width(), _pixmap.height()) * 3;
	resize(width, width);
}

void CanvasView::setScale(double value)
{
	if (_scale != value)
	{
		_scale = value;
		updateTransforms();
		emit scaleChanged(value);
	}
}

void CanvasView::setRotation(double value)
{
	if (_rotation != value)
	{
		_rotation = value;
		updateTransforms();
		emit rotationChanged(value);
	}
}

void CanvasView::setTranslation(const QPoint &value)
{
	if (_translation != value)
	{
		_translation = value;
		updateTransforms();
		emit translationChanged(value);
	}
}

void CanvasView::setTool(const QString &name)
{
	Tool *tool = createTool(appController()->modules(), controller()->workspace()->modules(), controller()->modules(), name, this);
	setTool(tool);
}

void CanvasView::setTool(Tool *tool)
{
	_tool = tool;
	if (tool)
	{
		connect(tool, SIGNAL(requestUpdate(QPointSet)), this, SLOT(updateTiles(QPointSet)));
		connect(tool, SIGNAL(requestUpdate(QHash<QPoint,QRect>)), this, SLOT(updateTiles(QHash<QPoint,QRect>)));
	}
}

void CanvasView::updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects)
{
	PAINTFIELD_CALC_SCOPE_ELAPSED_TIME;
	
	CanvasRenderer renderer;
	renderer.setTool(_tool);
	
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
		
		QPainter painter(&_pixmap);
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		
		drawMLImageFast(&painter, tilePos + rect.topLeft(), image);
		
		painter.end();
		
		QRect mappedRect = _transformFromScene.mapRect(QRectF(rect.translated(tilePos))).toAlignedRect();
		
		PAINTFIELD_DEBUG << _transformFromScene;
		
		repaint(mappedRect);
	}
}

void CanvasView::updateTransforms()
{
	_navigatorTransform = makeTransform(_scale, _rotation, _translation);
	_transformFromScene = QTransform::fromTranslate(- _pixmap.width() / 2, - _pixmap.height() / 2) * _navigatorTransform * QTransform::fromTranslate(geometry().width() / 2, geometry().height() / 2);
	_transformToScene = _transformFromScene.inverted();
	update();
}

void CanvasView::keyPressEvent(QKeyEvent *event)
{
	if (_tool)
		_tool->toolEvent(event);
}

void CanvasView::keyReleaseEvent(QKeyEvent *event)
{
	if (_tool)
		_tool->toolEvent(event);
}

void CanvasView::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (_tool)
		event->setAccepted(sendCanvasMouseEvent(event));
}

void CanvasView::mousePressEvent(QMouseEvent *event)
{
	if (_tool)
		event->setAccepted(sendCanvasTabletEvent(event) || sendCanvasMouseEvent(event));
}

void CanvasView::mouseMoveEvent(QMouseEvent *event)
{
	if (_tool)
		event->setAccepted(sendCanvasTabletEvent(event) || sendCanvasMouseEvent(event));
}

void CanvasView::mouseReleaseEvent(QMouseEvent *event)
{
	if (_tool)
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

	TabletInputData data(event->hiResGlobalPos(), event->pressure(), event->rotation(), event->tangentialPressure(), Vec2D(event->xTilt(), event->yTilt()));
	WidgetTabletEvent widgetTabletEvent(toNewEventType(event->type()), event->globalPos(), event->pos(), data, event->modifiers());

	customTabletEvent(&widgetTabletEvent);

	event->setAccepted(widgetTabletEvent.isAccepted());
}

void CanvasView::customTabletEvent(WidgetTabletEvent *event)
{
	if (_tool)
		event->setAccepted(sendCanvasTabletEvent(event));
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
	
	CanvasMouseEvent canvasEvent(toCanvasEventType(event->type()), event->globalPos(), event->posF() * _transformToScene, event->modifiers());
	_tool->toolEvent(&canvasEvent);
	return canvasEvent.isAccepted();
}

bool CanvasView::sendCanvasTabletEvent(WidgetTabletEvent *event)
{
	TabletInputData data = event->globalData;
	Vec2D globalPos = data.pos;
	data.pos += Vec2D(event->posInt - event->globalPosInt);
	data.pos *= _transformToScene;
	
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
	_tool->toolEvent(&canvasEvent);
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
		_mousePressure = 1.0;
	if (type == EventCanvasTabletRelease)
		_mousePressure = 0.0;
	
	TabletInputData data(mouseEvent->posF() * _transformToScene, _mousePressure, 0, 0, Vec2D(0));
	CanvasTabletEvent tabletEvent(type, mouseEvent->globalPos(), mouseEvent->globalPos(), data, mouseEvent->modifiers());
	_tool->toolEvent(&tabletEvent);
	return tabletEvent.isAccepted();
}

void CanvasView::resizeEvent(QResizeEvent *event)
{
	updateTransforms();
	event->accept();
}

void CanvasView::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setTransform(_transformFromScene);
	painter.drawPixmap(0, 0, _pixmap);
}


}
