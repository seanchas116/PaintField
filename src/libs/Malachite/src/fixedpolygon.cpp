#include "private/clipper.hpp"
#include "fixedpolygon.h"

namespace Malachite
{

FixedPolygon::FixedPolygon(const Polygon &polygon) :
    FixedPolygon(polygon.size())
{
	int size = polygon.size();
	
	FixedPoint *dst = data();
	const Vec2D *src = polygon.constData();
	
	for (int i = 0; i < size; ++i)
		*dst++ = FixedPoint(*src++);
}

void FixedPolygon::translate(const FixedPoint &delta)
{
	FixedPoint *p = data();
	
	for (int i = 0; i < size(); ++i)
		*p++ += delta;
}

FixedPolygon FixedPolygon::fromRect(const QRect &rect)
{
	FixedPolygon result(4);
	result[0] = FixedPoint(rect.left(), rect.top());
	result[1] = FixedPoint(rect.left() + rect.width(), rect.top());
	result[2] = FixedPoint(rect.left() + rect.width(), rect.top() + rect.height());
	result[3] = FixedPoint(rect.left(), rect.top() + rect.height());
	return result;
}

FixedMultiPolygon FixedMultiPolygon::fromPolygons(const MultiPolygon &polygons)
{
	FixedMultiPolygon result;
	foreach (const Polygon &polygon, polygons)
	{
		result << polygon;
	}
	return result;
}

QRectF FixedMultiPolygon::boundingRect() const
{
	FixedMultiPolygon polys = *this;
	
	for (int i = 0; i < polys.size(); ++i)
	{
		if (polys.at(i).size() == 0)
			polys.remove(i);
	}
	
	if (polys.size() == 0)
		return QRectF();
	
	FixedPoint min, max;
	min = max = polys.at(0).at(0);
	
	foreach (const FixedPolygon &poly, polys)
	{
		foreach (const FixedPoint &p, poly)
		{
			if (p.x < min.x)
				min.x = p.x;
			if (p.y < min.y)
				min.y = p.y;
			if (p.x > max.x)
				max.x = p.x;
			if (p.y > max.y)
				max.y = p.y;
		}
	}
	
	Vec2D xy, wh;
	xy = min.toMLVec2D();
	wh = max.toMLVec2D() - xy;
	
	return QRectF(xy.x(), xy.y(), wh.x(), wh.y());
}

using namespace ClipperLib;

FixedMultiPolygon operator|(const FixedMultiPolygon &polygons1, const FixedMultiPolygon &polygons2)
{
	if (polygons1.size() == 0)
		return polygons2;
	
	if (polygons2.size() == 0)
		return polygons1;
	
	FixedMultiPolygon result;
	
	Clipper clipper;
	clipper.AddPolygons(blindCast<const Polygons>(polygons1), ptSubject);
	clipper.AddPolygons(blindCast<const Polygons>(polygons2), ptClip);
	clipper.Execute(ctUnion, blindCast<Polygons>(result));
	
	return result;
}

FixedMultiPolygon operator&(const FixedMultiPolygon &polygons1, const FixedMultiPolygon &polygons2)
{
	if (polygons1.size() == 0 || polygons2.size() == 0)
		return FixedMultiPolygon();
	
	FixedMultiPolygon result;
	
	Clipper clipper;
	clipper.AddPolygons(blindCast<const Polygons>(polygons1), ptSubject);
	clipper.AddPolygons(blindCast<const Polygons>(polygons2), ptClip);
	clipper.Execute(ctIntersection, blindCast<Polygons>(result));
	
	return result;
}

FixedMultiPolygon operator^(const FixedMultiPolygon &polygons1, const FixedMultiPolygon &polygons2)
{
	if (polygons1.size() == 0)
		return polygons2;
	
	if (polygons2.size() == 0)
		return polygons1;
	
	FixedMultiPolygon result;
	
	Clipper clipper;
	clipper.AddPolygons(blindCast<const Polygons>(polygons1), ptSubject);
	clipper.AddPolygons(blindCast<const Polygons>(polygons2), ptClip);
	clipper.Execute(ctXor, blindCast<Polygons>(result));
	
	return result;
}

FixedMultiPolygon operator-(const FixedMultiPolygon &polygons1, const FixedMultiPolygon &polygons2)
{
	if (polygons1.size() == 0)
		return FixedMultiPolygon();
	
	if (polygons2.size() == 0)
		return polygons1;
	
	FixedMultiPolygon result;
	
	Clipper clipper;
	clipper.AddPolygons(blindCast<const Polygons>(polygons1), ptSubject);
	clipper.AddPolygons(blindCast<const Polygons>(polygons2), ptClip);
	clipper.Execute(ctDifference, blindCast<Polygons>(result));
	
	return result;
}

}
