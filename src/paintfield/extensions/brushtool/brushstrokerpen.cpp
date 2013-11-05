#include <float.h>

#include "brushstrokerpen.h"

using namespace Malachite;

namespace PaintField {

Polygon BrushStrokerPen::calcTangentQuadrangle(double r1, const Vec2D &k1, double r2, const Vec2D &k2, double d)
{
	double s = (r2 - r1) / d;
	double ss = s*s;
	
	if (ss >= 1.0)
		return Polygon();
	
	double c = sqrt(1.0 - ss);
	
	double ns = -s;
	double nc = -c;
	
	Vec2D k1_k2 = k2 - k1;
	Vec2D k1_k2_0 = k1_k2.extractX();
	Vec2D k1_k2_1 = k1_k2.extractY();
	
	Vec2D vp = Vec2D(ns, c) * k1_k2_0 + Vec2D(nc, ns) * k1_k2_1;
	Vec2D vq = Vec2D(ns, nc) * k1_k2_0 + Vec2D(c, ns) * k1_k2_1;
	
	Vec2D t = Vec2D(r1, r2) / d;
	Vec2D t1 = t.extractX();
	Vec2D t2 = t.extractY();
	
	Polygon poly(4);
	poly[0] = k1 + vq * t1;
	poly[1] = k2 + vq * t2;
	poly[2] = k2 + vp * t2;
	poly[3] = k1 + vp * t1;
	
	return poly;
}

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

void BrushStrokerPen::drawInterval(const Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd)
{
	//PAINTFIELD_CALC_SCOPE_ELAPSED_TIME;
	
	double totalLength;
	QVector<double> lengths = calcLength(polygon, &totalLength);
	
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
		
		shape = shape | FixedMultiPolygon(calcTangentQuadrangle(prevRadius, polygon.at(i-1), radius, polygon.at(i), lengths.at(i-1)));
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
