#include <QDebug>

#include "paintengine.h"

namespace Malachite
{

static Brush defaultBrush = Brush(Color::fromRgbValue(0, 0, 0));

PaintEngine::PaintEngine()
{
	_state.blendMode = BlendMode::Normal;
	_state.brush = defaultBrush;
	_state.opacity = 1.0;
	_state.imageTransformType = Malachite::ImageTransformTypeBicubic;
}

void PaintEngine::drawPreTransformedPolygons(const MultiPolygon &polygons)
{
	drawPreTransformedPolygons(FixedMultiPolygon::fromPolygons(polygons));
}

void PaintEngine::drawPolygons(const MultiPolygon &polygons)
{
	drawPreTransformedPolygons(polygons * state()->shapeTransform);
}

void PaintEngine::drawPath(const QPainterPath &path)
{
	drawPreTransformedPolygons(FixedMultiPolygon::fromQPainterPath(path * state()->shapeTransform));
}

void PaintEngine::drawEllipse(double x, double y, double rx, double ry)
{
	QPainterPath path;
	path.addEllipse(QPointF(x, y), rx, ry);
	drawPath(path);
}

void PaintEngine::drawRect(double x, double y, double width, double height)
{
	Polygon polygon(4);
	
	polygon[0] = Vec2D(x, y);
	polygon[1] = Vec2D(x + width, y);
	polygon[2] = Vec2D(x + width, y + height);
	polygon[3] = Vec2D(x, y + height);
	
	drawPolygons(polygon);
}

void PaintEngine::drawPreTransformedSurface(const QPoint &point, const Surface &surface)
{
	foreach (const QPoint &key, surface.keys())
		drawPreTransformedImage(point + key * Surface::tileWidth(), surface.tile(key));
}

void PaintEngine::drawSurface(const Vec2D &point, const Surface &surface)
{
	QTransform transform = _state.shapeTransform;
	if (transform.type() <= QTransform::TxTranslate)
	{
		Vec2D offset = point + Vec2D(transform.dx(), transform.dy());
		QPoint rounded(std::round(offset.x()), std::round(offset.y()));
		
		if (offset.x() == rounded.x() && offset.y() == rounded.y())
		{
			drawPreTransformedSurface(rounded, surface);
			return;
		}
	}
	
	pushState();
	
	_state.shapeTransform = QTransform::fromTranslate(point.x(), point.y()) * _state.shapeTransform;
	_state.brush = Brush(surface);
	
	qDebug() << surface.keys();
	
	foreach (const QPoint &key, surface.keys())
	{
		Vec2D relativePos = key * Surface::tileWidth();
		drawRect(relativePos.x(), relativePos.y(), Surface::tileWidth(), Surface::tileWidth());
	}
	
	popState();
}

void PaintEngine::drawImage(const Vec2D &point, const Image &image)
{
	QTransform transform = _state.shapeTransform;
	if (transform.type() <= QTransform::TxTranslate)
	{
		Vec2D offset = point + Vec2D(transform.dx(), transform.dy());
		QPoint rounded(std::round(offset.x()), std::round(offset.y()));
		
		if (offset.x() == rounded.x() && offset.y() == rounded.y())
		{
			drawPreTransformedImage(rounded, image);
			return;
		}
	}
	
	pushState();
	
	_state.brush = Brush(image);
	_state.brush.setSpreadType(Malachite::SpreadTypeReflective);
	
	_state.shapeTransform = QTransform::fromTranslate(point.x(), point.y()) * _state.shapeTransform;
	
	drawRect(0, 0, image.width(), image.height());
	
	popState();
}

void PaintEngine::pushState()
{
	_stateStack.push(_state);
}

void PaintEngine::popState()
{
	_state = _stateStack.pop();
}

}
