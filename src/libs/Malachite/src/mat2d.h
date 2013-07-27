#pragma once

//ExportName: Mat2D

#include "vec2d.h"

namespace Malachite
{

class MALACHITESHARED_EXPORT Mat2D
{
public:
	
	Mat2D() : _v1(1, 0), _v2(0, 1) {}
	Mat2D(const Vec2D &v1, const Vec2D &v2) : _v1(v1), _v2(v2) {}
	Mat2D(double a11, double a12, double a21, double a22) : _v1(a11, a21), _v2(a12, a22) {}
	Mat2D(const Mat2D &other) : _v1(other._v1), _v2(other._v2) {}
	
	double a11() const { return _v1.x(); }
	double a12() const { return _v2.x(); }
	double a21() const { return _v1.y(); }
	double a22() const { return _v2.y(); }
	
	double &ra11() { return _v1.rx(); }
	double &ra12() { return _v2.rx(); }
	double &ra21() { return _v1.ry(); }
	double &ra22() { return _v2.ry(); }
	
	Vec2D v1() const { return _v1; }
	Vec2D v2() const { return _v2; }
	
	Vec2D &rv1() { return _v1; }
	Vec2D &rv2() { return _v2; }
	
	double determinant() const
	{
		return a11() * a22() - a12() * a21();
	}
	
	double trace() const
	{
		return a11() + a22();
	}
	
	Mat2D transposedCofactorMat() const
	{
		return Mat2D(a22(), -a12(), -a21(), a11());
	}
	
private:
	
	Vec2D _v1, _v2;
};

inline Mat2D operator *(double scalar, const Mat2D &m)
{
	Mat2D r;
	r.rv1() = scalar * m.v1();
	r.rv2() = scalar * m.v2();
	return r;
}

inline Vec2D operator *(const Mat2D &m, const Vec2D &v)
{
	return m.v1() * v.extractX() + m.v2() * v.extractY();
}

inline Mat2D operator *(const Mat2D &m1, const Mat2D &m2)
{
	Mat2D r;
	r.rv1() = m1 * m2.v1();
	r.rv2() = m1 * m2.v2();
	return r;
}

}
