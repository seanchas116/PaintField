#pragma once

//ExportName: FixedPolygon

#include <QVector>
#include "misc.h"
#include "polygon.h"

namespace Malachite
{

struct MALACHITESHARED_EXPORT FixedPoint
{
	enum
	{
		SubpixelWidth = 8,
		SubpixelPrecision = 1 << SubpixelWidth
	};
	
	int64_t intToFixed(int64_t x)
	{
		uint64_t data = blindCast<uint64_t>(x);
		uint64_t sign = data & (uint64_t(1) << 63);
		data <<= SubpixelWidth;
		data |= sign;
		return blindCast<int64_t>(data);
	}
	
	FixedPoint() {}
	FixedPoint(int x, int y) : x(intToFixed(x)), y(intToFixed(y)) {}
	FixedPoint(const QPoint &p) : FixedPoint(p.x(), p.y()) {}
	FixedPoint(double x, double y) : FixedPoint(Vec2D(x, y)) {}
	
	FixedPoint(const Vec2D &p)
	{
		Vec2D v = p * double(SubpixelPrecision);
		x = round(v.x());
		y = round(v.y());
	}
	
	static FixedPoint fromRawData(int64_t x, int64_t y)
	{
		FixedPoint result;
		result.x = x;
		result.y = y;
		return result;
	}
	
	FixedPoint &operator+=(const FixedPoint &p)
	{
		x += p.x;
		y += p.y;
		return *this;
	}
	
	Vec2D toMLVec2D() const
	{
		Vec2D result(x, y);
		result *= (1.0 / SubpixelPrecision);
		return result;
	}
	
	int64_t x, y;	// For compatibility with ClipperLib
};

class MALACHITESHARED_EXPORT FixedPolygon : public QVector<FixedPoint>
{
public:
	FixedPolygon() : QVector<FixedPoint>() {}
	FixedPolygon(int size) : QVector<FixedPoint>(size) {}
	FixedPolygon(const Polygon &polygon);
	
	void translate(const FixedPoint &delta);
	
	static FixedPolygon fromRect(const QRectF &rect) { return Polygon::fromRect(rect); }
	static FixedPolygon fromRect(const QRect &rect);
};

class MALACHITESHARED_EXPORT FixedMultiPolygon : public QVector<FixedPolygon>
{
public:
	FixedMultiPolygon() : QVector<FixedPolygon>() {}
	FixedMultiPolygon(int size) : QVector<FixedPolygon>(size) {}
	FixedMultiPolygon(const FixedPolygon &polygon) : FixedMultiPolygon(1) { operator[](0) = polygon; }
	
	void translate(const FixedPoint &delta)
	{
		for (auto i = begin(); i != end(); ++i)
			i->translate(delta);
	}
	
	QRectF boundingRect() const;
	
	static FixedMultiPolygon fromPolygons(const MultiPolygon &polygons);
	static FixedMultiPolygon fromQPainterPath(const QPainterPath &path) { return fromPolygons(MultiPolygon::fromQPainterPath(path)); }
};

MALACHITESHARED_EXPORT FixedMultiPolygon operator|(const FixedMultiPolygon &polygons1, const FixedMultiPolygon &polygons2);
MALACHITESHARED_EXPORT FixedMultiPolygon operator&(const FixedMultiPolygon &polygons1, const FixedMultiPolygon &polygons2);
MALACHITESHARED_EXPORT FixedMultiPolygon operator^(const FixedMultiPolygon &polygons1, const FixedMultiPolygon &polygons2);
MALACHITESHARED_EXPORT FixedMultiPolygon operator-(const FixedMultiPolygon &polygons1, const FixedMultiPolygon &polygons2);

}

