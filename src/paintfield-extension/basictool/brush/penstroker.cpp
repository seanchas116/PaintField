#include "paintfield-core/debug.h"

#include "penstroker.h"

using namespace Malachite;

namespace PaintField
{

Polygon PenStroker::calcTangentQuadrangle(double radius1, const Vec2D &center1, double radius2, const Vec2D &center2)
{
	double r1, r2;
	Vec2D k1, k2, k2k1;
	
	if (radius1 < radius2)
	{
		r1 = radius1;
		r2 = radius2;
		k1 = center1;
		k2 = center2;
	}
	else
	{
		r1 = radius2;
		r2 = radius1;
		k1 = center2;
		k2 = center1;
	}
	
	k2k1 = k1 - k2;
	
	double dd = vecSqLength(k2k1);
	
	double dcos = r2 - r1;
	double dsin = sqrt(dd - dcos * dcos);
	
	Vec2D cs(dcos, dsin);
	Vec2D sc(dsin, dcos);
	Vec2D cns(dcos, -dsin);
	Vec2D nsc(-dsin, dcos);
	
	double f1 = r1 / dd;
	double f2 = r2 / dd;
	
	Vec2D k1p1, k1q1, k2p2, k2q2;
	
	k1p1.x = vecDot(cns, k2k1);
	k1p1.y = vecDot(sc, k2k1);
	k1p1 *= f1;
	
	k1q1.x = vecDot(cs, k2k1);
	k1q1.y = vecDot(nsc, k2k1);
	k1q1 *= f1;
	
	k2q2.x = vecDot(cns, k2k1);
	k2q2.y = vecDot(sc, k2k1);
	k2q2 *= f2;
	
	k2p2.x = vecDot(cs, k2k1);
	k2p2.y = vecDot(nsc, k2k1);
	k2p2 *= f2;
	
	Polygon poly(4);
	
	poly[0] = k1 + k1p1;
	poly[1] = k2 + k2q2;
	poly[2] = k2 + k2p2;
	poly[3] = k1 + k1q1;
	
	return poly;
}

void PenStroker::drawFirst(const TabletInputData &data)
{
	_drawnShapes.clear();
	_radiusBase = setting()->diameter * 0.5;
	
	double radius = data.pressure * _radiusBase;
	
	QPainterPath ellipsePath;
	ellipsePath.addEllipse(data.pos, radius, radius);
	
	drawShape(FixedMultiPolygon::fromQPainterPath(ellipsePath));
}

void PenStroker::drawInterval(const Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd)
{
	PAINTFIELD_CALC_SCOPE_ELAPSED_TIME;
	
	double totalLength;
	QVector<double> lengths = calcLength(polygon, &totalLength);
	
	if (totalLength == 0)
		return;
	
	double pressureNormalized = (dataEnd.pressure - dataStart.pressure) / totalLength;
	double pressure = dataStart.pressure;
	
	FixedMultiPolygon shape;
	
	double radius = pressure * _radiusBase;
	
	for (int i = 1; i < polygon.size(); ++i)
	{
		pressure += pressureNormalized * lengths.at(i-1);
		
		double prevRadius = radius;
		radius = pressure * _radiusBase;
		
		shape = shape | FixedMultiPolygon(calcTangentQuadrangle(prevRadius, polygon.at(i-1), radius, polygon.at(i)));
		
		QPainterPath ellipsePath;
		ellipsePath.addEllipse(polygon.at(i), radius, radius);
		shape = shape | FixedMultiPolygon::fromQPainterPath(ellipsePath);
	}
	
	drawShape(shape);
}

void PenStroker::drawShape(const FixedMultiPolygon &shape)
{
	Surface drawSurface = originalSurface();
	SurfaceEditor drawSurfaceEditor(&drawSurface);
	
	QPointSet keys = Surface::keysForRect(shape.boundingRect().toAlignedRect());
	
	QPointHashToQRect keysWithRects;
	
	for (const QPoint &key : keys)
	{
		FixedMultiPolygon dividedShape = shape & FixedPolygon::fromRect(Surface::keyToRect(key));
		
		if (dividedShape.size())
		{
			dividedShape.translate(key * -Surface::TileSize);
			
			QRect dividedBoundingRect = dividedShape.boundingRect().toAlignedRect();
			
			FixedMultiPolygon drawShape = _drawnShapes[key] | dividedShape;
			_drawnShapes[key] = drawShape;
			
			Painter painter(drawSurfaceEditor.tileRefForKey(key));
			painter.setArgb(argb());
			painter.setBlendMode(BlendModeSourceOver);
			painter.drawTransformedPolygons(drawShape);
			
			keysWithRects.insert(key, dividedBoundingRect);
		}
	}
	
	SurfaceEditor editor(surface());
	editor.replace(drawSurface, keys);
	addEditedKeys(keysWithRects);
}

/*
void PenStroker::drawInterval(const Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd)
{
	double totalLength;
	QVector<double> lengths = calcLength(polygon, &totalLength);
	
	if (totalLength == 0)
		return;
	
	double pressureNormalized = (dataEnd.pressure - dataStart.pressure) / totalLength;
	double pressure = dataStart.pressure;
	
	for (int i = 1; i < polygon.size(); ++i)
	{
		pressure += pressureNormalized * lengths.at(i-1);
		drawOne(polygon.at(i), pressure, true);
	}
}

void PenStroker::drawOne(const Vec2D &pos, double pressure, bool drawQuad)
{
	qDebug() << "pos" << pos.x << pos.y << "pressure" << pressure;
	
	double radius = pressure * setting()->diameter * 0.5;
	
	//radius = 0.5 * _radiusPrev + 0.5 * radius;
	
	qDebug() << "radius" << radius;
	
	Surface drawSurface = originalSurface();
	SurfacePainter painter(&drawSurface);
	painter.setArgb(argb());
	painter.setBlendMode(BlendModeSourceOver);
	
	// creating shape
	
	FixedMultiPolygon shape;
	
	QPainterPath ellipsePath;
	ellipsePath.addEllipse(pos, radius, radius);
	shape = FixedMultiPolygon::fromQPainterPath(ellipsePath);
	
	if (drawQuad)
	{
		shape = shape | FixedMultiPolygon(calcTangentQuadrangle(_radiusPrev, _posPrev, radius, pos));
	}
	
	// divide and draw shapes
	
	QPointSet keys = Surface::keysForRect(shape.boundingRect().toAlignedRect());
	
	QPointHashToQRect keysWithRects;
	
	for (const QPoint &key : keys)
	{
		FixedMultiPolygon dividedShape = shape & FixedPolygon::fromRect(Surface::keyToRect(key));
		
		QRect dividedBoundingRect = dividedShape.boundingRect().toAlignedRect();
		
		FixedMultiPolygon drawShape = _drawnShapes[key] | dividedShape;
		//drawShape = drawShape & FixedPolygon::fromRect(dividedBoundingRect);
		
		if (drawShape.size())
		{
			painter.drawTransformedPolygons(drawShape);
			_drawnShapes[key] = drawShape;
		}
		
		keysWithRects.insert(key, dividedBoundingRect);
	}
	
	_radiusPrev = radius;
	_posPrev = pos;
	
	SurfaceEditor editor(surface());
	editor.replace(drawSurface, keys);
	addEditedKeys(keysWithRects);
}*/

}
