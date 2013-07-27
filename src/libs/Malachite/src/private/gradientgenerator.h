#pragma once

#include "../vec2d.h"
#include "../pixel.h"

namespace Malachite
{

template <class T_Gradient, class T_Method, Malachite::SpreadType T_SpreadType>
class GradientGenerator
{
public:
	GradientGenerator(const T_Gradient *gradient, T_Method *method) :
		_gradient(gradient),
		_method(method)
	{}
	
	Pixel at(const Vec2D &p)
	{
		return _gradient->at(actualPosition(_method->position(p)));
	}
	
private:
	
	float actualPosition(float x) const
	{
		switch (T_SpreadType)
		{
		default:
		case Malachite::SpreadTypePad:
			return qBound(0.f, x, 1.f);
		case Malachite::SpreadTypeRepeat:
			return x - floorf(x);
		case Malachite::SpreadTypeReflective:
		{
			float f = floorf(x);
			float r = x - f;
			return (int)f % 2 ? 1.f - r : r;
		}
		}
	}
	
	const T_Gradient *_gradient;
	T_Method *_method;
};

class LinearGradientMethod
{
public:
	
	LinearGradientMethod(const Vec2D &start, const Vec2D &end) :
		a(start),
		ab(end - start)
	{
		Q_ASSERT(start != end);
		ab2inv = 1.0 / ab.lengthSquare();
	}
	
	float position(const Vec2D &p) const
	{
		return Vec2D::dot(p - 1, ab) * ab2inv;
	}
	
private:
	Vec2D a;
	Vec2D ab;
	double ab2inv;
};

/*
class RadialGradientMethod
{
public:
	
	RadialGradientMethod(const Vec2D &center, double radius) :
		c(center)
	{
		Q_ASSERT(radius > 0);
		rinv = 1.0 / radius;
	}
	
	float position(const Vec2D &p) const
	{
		Vec2D d = p - c;
		return Length(d) * rinv;
	}
	
private:
	Vec2D c;
	double rinv;
};*/

class RadialGradientMethod
{
public:
	
	RadialGradientMethod(const Vec2D &center, const Vec2D &radius) :
		c(center)
	{
		Q_ASSERT(radius.x() > 0 && radius.y() > 0);
		rinv = 1.0 / radius;
	}
	
	float position(const Vec2D &p) const
	{
		Vec2D d = p - c;
		return (d * rinv).length();
	}
	
private:
	Vec2D c, rinv;
};

/*
class FocalGradientMethod
{
public:
	
	FocalGradientMethod(const Vec2D &center, double radius, const Vec2D &focal) :
		o(center), f(focal), r(radius)
	{
		Q_ASSERT(r > 0);
		of = f - o;
		c = SqLength(of) - r * r;
		Q_ASSERT(c < 0);
	}
	
	float position(const Vec2D &p) const
	{
		Vec2D fp = p - f;
		double dot = Dot(of, fp);
		double fp2 = SqLength(fp);
		
		return fp2 / (sqrt(dot * dot - fp2 * c) - dot); 
	}
	
private:
	Vec2D o, f, of;
	double r, c;
};
*/

class FocalGradientMethod
{
public:
	
	FocalGradientMethod(const Vec2D &center, const Vec2D &radius, const Vec2D &focal) :
		o(center)
	{
		Q_ASSERT(radius.x() > 0 && radius.y() > 0);
		rinv = 1 / radius;
		
		of = (focal - center) * rinv;
		c = of.lengthSquare() - 1;
		Q_ASSERT(c < 0);
	}
	
	float position(const Vec2D &p) const
	{
		Vec2D op = (p - o) * rinv;
		Vec2D fp = op - of;
		
		double dot = Vec2D::dot(of, fp);
		double fp2 = fp.lengthSquare();
		
		return fp2 / (sqrt(dot * dot - fp2 * c) - dot); 
	}
	
private:
	Vec2D o, of, rinv;
	double c;
};

}

