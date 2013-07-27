#pragma once

//ExportName: ColorGradient

#include "color.h"
#include "vec2d.h"
#include <QMap>

namespace Malachite
{

class MALACHITESHARED_EXPORT ColorGradient
{
public:
	ColorGradient() {}
	virtual ~ColorGradient() {}
	
	virtual Pixel at(float x) const = 0;
	virtual ColorGradient *clone() const { return 0; }
};

class MALACHITESHARED_EXPORT ColorGradientCache : public ColorGradient
{
public:
	
	ColorGradientCache(ColorGradient *gradient, int sampleCount);
	
	Pixel at(float x) const
	{
		return _cache.at(roundf(x * _sampleCount));
	}
	
	int sampleCount() const { return _sampleCount; }
	
private:
	
	int _sampleCount;
	QVector<Pixel> _cache;
};

class MALACHITESHARED_EXPORT ArgbGradient : public ColorGradient
{
public:
	
	ArgbGradient() : ColorGradient() {}
	
	void addStop(float x, const Pixel &y) { _stops.insert(x, y); }
	void addStop(float x, const Color &color) { addStop(x, color.toPixel()); }
	void clear() { _stops.clear(); }
	
	Pixel at(float x) const;
	
	ColorGradient *clone() const { return new ArgbGradient(*this); }
	
private:
	
	QMap<float, Pixel> _stops;
};

struct LinearGradientShape
{
	LinearGradientShape() {}
	
	LinearGradientShape(const Vec2D &start, const Vec2D &end) :
		start(start), end(end) {}
	
	bool transformable(const QTransform &transform) const
	{
		if (transform.isIdentity())
			return true;
		
		if (transform.isAffine() && transform.m12() == 0 && transform.m21() == 0 && transform.m11() == transform.m22())
			return true;
		
		return false;
	}
	
	void transform(const QTransform &transform)
	{
		start *= transform;
		end *= transform;
	}
	
	Vec2D start, end;
};

struct RadialGradientShape
{
	RadialGradientShape() {}
	
	RadialGradientShape(const Vec2D &center, double radius, const Vec2D &focal) :
		center(center), focal(focal), radius(radius) {}
	
	RadialGradientShape(const Vec2D &center, const Vec2D &radius, const Vec2D &focal) :
		center(center), focal(focal), radius(radius) {}
	
	RadialGradientShape(const Vec2D &center, double radius) :
		center(center), focal(center), radius(radius) {}
	
	RadialGradientShape(const Vec2D &center, const Vec2D &radius) :
		center(center), focal(center), radius(radius) {}
	
	bool transformable(const QTransform &transform) const
	{
		if (transform.isIdentity())
			return true;
		
		if (transform.isAffine() && transform.m12() == 0 && transform.m21() == 0)
			return true;
		
		return false;
	}
	
	void transform(const QTransform &transform)
	{
		center *= transform;
		focal *= transform;
		radius *= Vec2D(transform.m11(), transform.m22());
	}
	
	Vec2D center, focal, radius;
};


}

Q_DECLARE_METATYPE(Malachite::LinearGradientShape)
Q_DECLARE_METATYPE(Malachite::RadialGradientShape)

