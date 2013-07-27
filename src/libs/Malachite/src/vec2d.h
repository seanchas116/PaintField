#pragma once

//ExportName: Vec2D

#include <QPointF>
#include <QSizeF>
#include <QTransform>
#include <cmath>
#include "global.h"
#include "vector_sse.h"

namespace Malachite
{

class MALACHITESHARED_EXPORT Vec2D : public Vector<double, 2>
{
public:
	
	typedef Vector<double, 2> super;
	
	Vec2D() : super(0) {}
	Vec2D(const super &other) : super(other) {}
	Vec2D(double s) : super(s) {}
	Vec2D(double x, double y)
	{
		at(0) = x;
		at(1) = y;
	}
	Vec2D(const QPointF &p) : Vec2D(p.x(), p.y()) {}
	Vec2D(const QPoint &p) : Vec2D(p.x(), p.y()) {}
	Vec2D(const QSizeF &s) : Vec2D(s.width(), s.height()) {}
	Vec2D(const QSize &s) : Vec2D(s.width(), s.height()) {}
	
	operator QPointF&() { return *reinterpret_cast<QPointF *>(this); }
	operator const QPointF&() const { return *reinterpret_cast<const QPointF *>(this); }
	operator QSizeF&() { return *reinterpret_cast<QSizeF *>(this); }
	operator const QSizeF&() const { return *reinterpret_cast<const QSizeF *>(this); }
	
	QPoint toQPoint() const { return QPointF(*this).toPoint(); }
	
	double x() const { return at(0); }
	double y() const { return at(1); }
	void setX(double x) { at(0) = x; }
	void setY(double y) { at(1) = y; }
	double &rx() { return at(0); }
	double &ry() { return at(1); }
	
	Vec2D extractX() const { return extract(0); }
	Vec2D extractY() const { return extract(1); }
	
	Vec2D floor() const { return Vec2D(::floor(x()), ::floor(y())); }
	Vec2D ceil() const { return Vec2D(::ceil(x()), ::ceil(y())); }
	
	static double dot(const Vec2D &v1, const Vec2D &v2)
	{
		Vec2D v = v1 * v2;
		return v.x() + v.y();
	}
	
	double length() const
	{
		return ::hypot(x(), y());
	}
	
	double lengthSquare() const
	{
		return dot(*this, *this);
	}
	
	double arg() const
	{
		return ::atan2(y(), x());
	}
	
	Vec2D &operator*=(const QTransform &transform);
	
	const Vec2D operator+(const Vec2D &v) const { return super::operator+(v); }
	const Vec2D operator-(const Vec2D &v) const { return super::operator-(v); }
	const Vec2D operator*(const Vec2D &v) const { return super::operator*(v); }
	const Vec2D operator/(const Vec2D &v) const { return super::operator/(v); }
	
	ML_IMPL_VECTOR_OPERATORS(Vec2D, double)
};

ML_IMPL_VECTOR_OPERATORS_GLOBAL(inline Vec2D, Vec2D, double)

inline Vec2D operator*(const Vec2D &v, const QTransform &transform)
{
	Vec2D r;
	transform.map(v.x(), v.y(), &r.rx(), &r.ry());
	return r;
}

inline Vec2D &Vec2D::operator*=(const QTransform &transform)
{
	return *this = *this * transform;
}

}

Q_DECLARE_METATYPE(Malachite::Vec2D)

