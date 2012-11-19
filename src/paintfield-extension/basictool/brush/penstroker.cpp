#include <float.h>
#include "paintfield-core/debug.h"

#include "penstroker.h"

using namespace Malachite;

namespace PaintField
{

PenStroker::PenStroker(Surface *surface, const BrushSetting *setting, const Vec4F &argb) :
	Stroker(surface, setting, argb)
{
}

Polygon PenStroker::calcTangentQuadrangle(double r1, const Vec2D &k1, double r2, const Vec2D &k2, double d)
{
	double s = (r2 - r1) / d;
	double ss = s*s;
	
	if (ss >= 1.0)
		return Polygon();
	
	double c = sqrt(1.0 - ss);
	
	double ns = -s;
	double nc = -c;
	
	Vec2D k1_k2 = k2 - k1;
	Vec2D k1_k2_0 = k1_k2.extract0();
	Vec2D k1_k2_1 = k1_k2.extract1();
	
	Vec2D vp = Vec2D(ns, c) * k1_k2_0 + Vec2D(nc, ns) * k1_k2_1;
	Vec2D vq = Vec2D(ns, nc) * k1_k2_0 + Vec2D(c, ns) * k1_k2_1;
	
	Vec2D t = Vec2D(r1, r2) / d;
	Vec2D t1 = t.extract0();
	Vec2D t2 = t.extract1();
	
	Polygon poly(4);
	poly[0] = k1 + vq * t1;
	poly[1] = k2 + vq * t2;
	poly[2] = k2 + vp * t2;
	poly[3] = k1 + vp * t1;
	
	return poly;
}

/*
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
}*/

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
	//PAINTFIELD_CALC_SCOPE_ELAPSED_TIME;
	
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
		if (lengths.at(i-1) == 0)
			continue;
		
		pressure += pressureNormalized * lengths.at(i-1);
		
		double prevRadius = radius;
		radius = pressure * _radiusBase;
		
		shape = shape | FixedMultiPolygon(calcTangentQuadrangle(prevRadius, polygon.at(i-1), radius, polygon.at(i), lengths.at(i-1)));
		//shape = shape | FixedPolygon(Polygon::fromEllipse(polygon.at(i), Vec2D(radius)));
		
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
	editor.replace(drawSurface, keysWithRects.keys().toSet());
	addEditedKeys(keysWithRects);
}

}
