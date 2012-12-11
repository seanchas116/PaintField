#include <QtGui>

#include "widgets/vanishingscrollbar.h"
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
    _scrollBarX(new VanishingScrollBar(Qt::Horizontal, this)),
    _scrollBarY(new VanishingScrollBar(Qt::Vertical, this)),
    _canvas(canvas),
    _document(canvas->document()),
    _pixmap(_document->size())
{
	setMouseTracking(true);
	
	connect(layerModel(), SIGNAL(tilesUpdated(QPointSet)), this, SLOT(updateTiles(QPointSet)));
	updateTiles(layerModel()->document()->tileKeys());
	updateTransforms();
	
	//int width = qMax(_pixmap.width(), _pixmap.height()) * 3;
	//resize(width, width);
	moveScrollBars();
	
	connect(_scrollBarX, SIGNAL(valueChanged(int)), this, SLOT(onScrollBarXChanged(int)));
	connect(_scrollBarY, SIGNAL(valueChanged(int)), this, SLOT(onScrollBarYChanged(int)));
}

CanvasView::~CanvasView()
{}

void CanvasView::setScale(double value)
{
	if (_scale != value)
	{
		_scale = value;
		updateTransforms();
		emit scaleChanged(value);
	}
}

void CanvasView::setViewScale(double scale)
{
	if (_scale != scale)
	{
		_scale = scale;
		emit scaleChanged(scale);
		
		QPoint translation = _backupTranslation * (scale / _backupScale);
		
		if (_translation != translation)
		{
			_translation = translation;
			emit translationChanged(translation);
		}
		
		updateTransforms();
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

void CanvasView::setViewRotation(double rotation)
{
	if (_rotation != rotation)
	{
		_rotation = rotation;
		emit rotationChanged(rotation);
		
		QTransform transform;
		transform.rotate(rotation - _backupRotation);
		
		QPoint translation = transform.map(_backupTranslation);
		
		if (_translation != translation)
		{
			_translation = translation;
			emit translationChanged(_translation);
		}
		
		updateTransforms();
	}
}

void CanvasView::setTranslation(const QPoint &value)
{
	QPoint newValue;
	
	newValue.setX(qBound(-_maxAbsTranslation.x(), value.x(), _maxAbsTranslation.x()));
	newValue.setY(qBound(-_maxAbsTranslation.y(), value.y(), _maxAbsTranslation.y()));
	
	if (_translation != newValue)
	{
		_translation = newValue;
		
		_backupTranslation = newValue;
		_backupScale = _scale;
		_backupRotation = _rotation;
		
		updateTransforms();
		emit translationChanged(newValue);
	}
}

void CanvasView::setTool(Tool *tool)
{
	_tool.reset(tool);
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
		
		QRect mappedRect = _transformFromScene.mapRect(QRectF(rect.translated(tilePos))).toAlignedRect();
		
		repaint(mappedRect);
	}
}

void CanvasView::updateTransforms()
{
	QTransform transform;
	
	if (_translation != QPoint())
		transform.translate(_translation.x(), _translation.y());
	
	if (_scale != 1.0)
		transform.scale(_scale, _scale);
	
	if (_rotation)
		transform.rotate(_rotation);
	
	_navigatorTransform = transform;
	//_navigatorTransform = makeTransform(_scale, _rotation, _translation);
	_transformFromScene = QTransform::fromTranslate(- _pixmap.width() / 2, - _pixmap.height() / 2) * _navigatorTransform * QTransform::fromTranslate(geometry().width() / 2, geometry().height() / 2);
	_transformToScene = _transformFromScene.inverted();
	
	updateScrollBarRange();
	updateScrollBarValue();
	
	update();
}

void CanvasView::onScrollBarXChanged(int value)
{
	int x = -(value - _maxAbsTranslation.x());
	setTranslation(x, _translation.y());
}

void CanvasView::onScrollBarYChanged(int value)
{
	int y = -(value - _maxAbsTranslation.y());
	setTranslation(_translation.x(), y);
}

void CanvasView::updateScrollBarValue()
{
	_scrollBarX->setValue(-_translation.x() + _maxAbsTranslation.x());
	_scrollBarY->setValue(-_translation.y() + _maxAbsTranslation.y());
}

void CanvasView::updateScrollBarRange()
{
	int radius = ceil(hypot(_pixmap.width(), _pixmap.height()) * _scale * 0.5);
	
	_maxAbsTranslation = QPoint(radius + this->width(), radius + this->height());
	
	_scrollBarX->setRange(0, 2 * _maxAbsTranslation.x());
	_scrollBarY->setRange(0, 2 * _maxAbsTranslation.y());
	_scrollBarX->setPageStep(this->width());
	_scrollBarY->setPageStep(this->height());
}

void CanvasView::moveScrollBars()
{
	int barWidthX = _scrollBarX->totalBarWidth();
	int barWidthY = _scrollBarY->totalBarWidth();
	
	auto widgetRect = QRect(QPoint(), geometry().size());
	
	auto scrollBarXRect = widgetRect.adjusted(0, widgetRect.height() - barWidthY, -barWidthX, 0);
	auto scrollBarYRect = widgetRect.adjusted(widgetRect.width() - barWidthX, 0, 0, -barWidthY);
	
	_scrollBarX->setGeometry(scrollBarXRect);
	_scrollBarY->setGeometry(scrollBarYRect);
	
	/*
	_scrollBarX->move(0, this->height() - barWidthX);
	_scrollBarX->resize(this->width() - barWidthY, barWidthX);
	_scrollBarY->move(this->width() - barWidthY, 0);
	_scrollBarY->resize(barWidthY, this->height() - barWidthX);
	*/
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

void CanvasView::wheelEvent(QWheelEvent *event)
{
	QAbstractSlider *scrollBar;
	
	if (event->orientation() == Qt::Horizontal)
		scrollBar = _scrollBarX;
	else
		scrollBar = _scrollBarY;
	
	scrollBar->setValue(scrollBar->value() - event->delta());
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
	moveScrollBars();
	event->accept();
}

void CanvasView::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	
	painter.setTransform(_transformFromScene);
	painter.drawPixmap(0, 0, _pixmap);
}


}
