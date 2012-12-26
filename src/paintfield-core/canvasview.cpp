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
    NavigatableArea(parent),
    _canvas(canvas),
    _document(canvas->document()),
    _pixmap(_document->size())
{
	setMouseTracking(true);
	setSceneSize(_document->size());
	
	// TODO: can tablet events set focus?
	setFocusPolicy(Qt::ClickFocus);
	
	setMouseTracking(true);
	
	connect(layerModel(), SIGNAL(tilesUpdated(QPointSet)), this, SLOT(updateTiles(QPointSet)));
	updateTiles(layerModel()->document()->tileKeys());
	
	connect(appController()->app(), SIGNAL(tabletActiveChanged(bool)), this, SLOT(onTabletActiveChanged(bool)));
	onTabletActiveChanged(appController()->app()->isTabletActive());
}

CanvasView::~CanvasView()
{
	qApp->restoreOverrideCursor();
}

void CanvasView::setTool(Tool *tool)
{
	_tool.reset(tool);
	
	if (tool)
	{
		connect(tool, SIGNAL(requestUpdate(QPointSet)), this, SLOT(updateTiles(QPointSet)));
		connect(tool, SIGNAL(requestUpdate(QHash<QPoint,QRect>)), this, SLOT(updateTiles(QHash<QPoint,QRect>)));
		
		if (tool->isCustomCursorEnabled())
			_toolCursor = QCursor(Qt::BlankCursor);
		else
			_toolCursor = tool->cursor();
		
		/*
		if (tool->isCustomCursorEnabled())
			setCursor(QCursor(Qt::BlankCursor));
		else
		{
			connect(tool, SIGNAL(cursorChanged(QCursor)), this, SLOT(onToolCursorChanged(QCursor)));
			setCursor(tool->cursor());
		}*/
	}
}

void CanvasView::updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects)
{
	PAINTFIELD_CALC_SCOPE_ELAPSED_TIME;
	
	CanvasRenderer renderer;
	renderer.setTool(_tool.data());
	
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

void CanvasView::enterEvent(QEvent *e)
{
	PAINTFIELD_DEBUG;
	super::enterEvent(e);
	qApp->setOverrideCursor(_toolCursor);
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
	if (_tool)
		event->setAccepted(sendCanvasMouseEvent(event));
}

void CanvasView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
		PAINTFIELD_DEBUG << "right clicked";
	
	onClicked();
	
	if (_tool)
		event->setAccepted(sendCanvasTabletEvent(event) || sendCanvasMouseEvent(event));
}

void CanvasView::mouseMoveEvent(QMouseEvent *event)
{
	if (_tool)
	{
		if (!_tabletActive)
			_customCursorPos = event->posF();
		
		addCustomCursorRectToRepaintRect();
		
		event->setAccepted(sendCanvasTabletEvent(event) || sendCanvasMouseEvent(event));
		
		repaintDesignatedRect();
	}
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
	
	if (_tool)
	{
		if (int(event->type()) == EventWidgetTabletMove)
		{
			_customCursorPos = event->globalData.pos + Vec2D(event->posInt - event->globalPosInt);
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
	_tool->toolEvent(&canvasEvent);
	
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
	
	TabletInputData data(mouseEvent->posF() * transformToScene(), _mousePressure, 0, 0, Vec2D(0));
	CanvasTabletEvent tabletEvent(type, mouseEvent->globalPos(), mouseEvent->globalPos(), data, mouseEvent->modifiers());
	_tool->toolEvent(&tabletEvent);
	return tabletEvent.isAccepted();
}

void CanvasView::addCustomCursorRectToRepaintRect()
{
	if (_tool->isCustomCursorEnabled())
	{
		addRepaintRect(_prevCustomCursorRect);
		
		auto rect = _tool->customCursorRect(_customCursorPos);
		_prevCustomCursorRect = rect;
		
		addRepaintRect(rect);
	}
}

void CanvasView::addRepaintRect(const QRect &rect)
{
	_repaintRect |= rect;
}

void CanvasView::repaintDesignatedRect()
{
	if (_repaintRect.isValid())
	{
		//PAINTFIELD_DEBUG << "repainting" << _repaintRect;
		repaint(_repaintRect);
		_repaintRect = QRect();
	}
}

void CanvasView::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	
	painter.setTransform(transformFromScene());
	painter.drawPixmap(0, 0, _pixmap);
	painter.setTransform(QTransform());
	
	if (_tool && _tool->isCustomCursorEnabled())
	{
		_tool->drawCustomCursor(&painter, _customCursorPos);
	}
}


}
