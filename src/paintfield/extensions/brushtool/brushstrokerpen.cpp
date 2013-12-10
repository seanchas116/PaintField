#include <float.h>

#include "brushstrokerpen.h"

using namespace Malachite;

namespace PaintField {

BrushStrokerPen::BrushStrokerPen(Surface *surface) :
	BrushStroker(surface)
{
}

void BrushStrokerPen::drawFirst(const TabletInputData &data)
{
	//_drawnShapes.clear();
	
	double radius = data.pressure * radiusBase();
	
	QPainterPath ellipsePath;
	ellipsePath.addEllipse(data.pos, radius, radius);
	
	drawShape(FixedMultiPolygon::fromQPainterPath(ellipsePath));
}

void BrushStrokerPen::drawInterval(
	const Polygon &polygon, const QVector<double> &lengths, double totalLength,
	const TabletInputData &dataStart, const TabletInputData &dataEnd)
{
	if (totalLength == 0)
		return;
	
	double pressureNormalized = (dataEnd.pressure - dataStart.pressure) / totalLength;
	double pressure = dataStart.pressure;
	
	FixedMultiPolygon shape;
	
	double radius = pressure * radiusBase();
	
	for (int i = 1; i < polygon.size(); ++i)
	{
		if (lengths.at(i-1) == 0)
			continue;
		
		pressure += pressureNormalized * lengths.at(i-1);
		
		double prevRadius = radius;
		radius = pressure * radiusBase();
		
		shape = shape | FixedMultiPolygon(tangentQuadrangle(prevRadius, polygon.at(i-1), radius, polygon.at(i), lengths.at(i-1)));
		//shape = shape | FixedPolygon(Polygon::fromEllipse(polygon.at(i), Vec2D(radius)));
		
		QPainterPath ellipsePath;
		ellipsePath.addEllipse(polygon.at(i), radius, radius);
		shape = shape | FixedMultiPolygon::fromQPainterPath(ellipsePath);
	}
	
	drawShape(shape);
}

void BrushStrokerPen::drawShape(const FixedMultiPolygon &shape)
{
	QPointSet keys = Surface::rectToKeys(shape.boundingRect().toAlignedRect());
	
	QHash<QPoint, QRect> keysWithRects;
	
	for (const QPoint &key : keys)
	{
		FixedMultiPolygon dividedShape = shape & FixedPolygon::fromRect(Surface::keyToRect(key));
		
		if (dividedShape.size())
		{
			dividedShape.translate(key * -Surface::tileWidth());
			
			QRect dividedBoundingRect = dividedShape.boundingRect().toAlignedRect();
			
			FixedMultiPolygon drawShape = _drawnShapes[key] | dividedShape;
			_drawnShapes[key] = drawShape;
			
			auto tile = originalSurface().tile(key, Image());
			if (!tile.isValid())
				tile = Surface::createTile();
			
			Painter painter(&tile);
			painter.setPixel(_settings.eraser ? Pixel(1) : pixel());
			painter.setBlendMode(_settings.eraser ? BlendMode::DestinationOut : BlendMode::SourceOver);
			painter.drawPreTransformedPolygons(drawShape);
			
			keysWithRects.insert(key, dividedBoundingRect);
			
			surface()->setTile(key, tile);
		}
	}
	
	addEditedKeys(keysWithRects);
}

void BrushStrokerPen::loadSettings(const QVariantMap &settings)
{
	_settings.eraser = settings["eraser"].toBool();
}

QString BrushStrokerPenFactory::title() const
{
	return tr("Pen");
}

QVariantMap BrushStrokerPenFactory::defaultSettings() const
{
	QVariantMap settings;
	settings["eraser"] = false;
	return settings;
}

BrushStroker *BrushStrokerPenFactory::createStroker(Surface *surface)
{
	return new BrushStrokerPen(surface);
}

}
