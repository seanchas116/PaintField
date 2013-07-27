#pragma once

//ExportName: Polygon

#include "vec2d.h"
#include <QVector>
#include <QPolygonF>

namespace Malachite
{

class MALACHITESHARED_EXPORT Polygon : public QVector<Vec2D>
{
public:
	Polygon() : QVector<Vec2D>() {}
	Polygon(int size) : QVector<Vec2D>(size) {}
	Polygon(const QVector<QPointF> &points);
	
	static Polygon fromRect(const QRectF &rect);
	static Polygon fromEllipse(const Vec2D &center, const Vec2D &radius);
	
	Polygon &operator*=(const QTransform &transform);
	
	QPolygonF toQPolygonF() const;
};

class MALACHITESHARED_EXPORT MultiPolygon : public QVector<Polygon>
{
public:
	MultiPolygon() : QVector<Polygon>() {}
	MultiPolygon(int size) : QVector<Polygon>(size) {}
	MultiPolygon(const Polygon &polygon) : QVector<Polygon>(1) { operator[](0) = polygon; }
	
	static MultiPolygon fromQPainterPath(const QPainterPath &path);
	
	MultiPolygon &operator*=(const QTransform &transform);
};

inline MultiPolygon operator*(const MultiPolygon &polygons, const QTransform &transform)
{
	MultiPolygon result = polygons;
	result *= transform;
	return result;
}

}

