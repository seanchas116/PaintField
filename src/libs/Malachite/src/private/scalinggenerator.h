#pragma once

#include "../vec2d.h"
#include "../pixel.h"
#include "../division.h"
#include "../surface.h"
#include "../image.h"

namespace Malachite
{

template <class T_Image, Malachite::SpreadType T_SpreadType>
struct PosWrapperMethod;

template <class T_Image>
struct PosWrapperMethod<T_Image, Malachite::SpreadTypePad>
{
	static QPoint wrap(const QPoint &p, const QSize &size)
	{
		QPoint r;
		r.rx() = qBound(0, p.x(), size.width() - 1);
		r.ry() = qBound(0, p.y(), size.height() - 1);
		return r;
	}
};

template <class T_Image>
struct PosWrapperMethod<T_Image, Malachite::SpreadTypeRepeat>
{
	static QPoint wrap(const QPoint &p, const QSize &size)
	{
		QPoint r;
		r.rx() = IntDivision(p.x(), size.width()).rem();
		r.ry() = IntDivision(p.y(), size.height()).rem();
		return r;
	}
};

template <class T_Image>
struct PosWrapperMethod<T_Image, Malachite::SpreadTypeReflective>
{
	static QPoint wrap(const QPoint &p, const QSize &size)
	{
		QPoint r;
		IntDivision divX(p.x(), size.width());
		IntDivision divY(p.y(), size.height());
		r.rx() = (divX.quot() % 2) ? (size.width() - divX.rem() - 1) : divX.rem();
		r.ry() = (divY.quot() % 2) ? (size.height() - divY.rem() - 1) : divY.rem();
		return r;
	}
};

template <class T_SourceType, Malachite::SpreadType T_SpreadType>
class SourceWrapper;

template <Malachite::SpreadType T_SpreadType>
class SourceWrapper<Surface, T_SpreadType>
{
public:
	
	SourceWrapper(const Surface *src) : _src(src) {}
	
	Pixel pixel(const QPoint &p) const
	{
		return _src->pixel(p);
	}
	
	Pixel pixelDirect(const QPoint &p) const
	{
		return _src->pixel(p);
	}
	
private:
	
	const Surface *_src;
};

template <Malachite::SpreadType T_SpreadType>
class SourceWrapper<Bitmap<const Pixel>, T_SpreadType>
{
public:
	
	SourceWrapper(const Bitmap<const Pixel> *src) : _src(src) {}
	
	Pixel pixel(const QPoint &p) const
	{
		return _src->pixel(PosWrapperMethod<Image, T_SpreadType>::wrap(p, _src->size()));
	}
	
	Pixel pixelDirect(const QPoint &p) const
	{
		return _src->pixel(p);
	}
	
private:
	
	const Bitmap<const Pixel> *_src;
};

template <class T_Source, Malachite::SpreadType T_SpreadType>
class ScalingGeneratorNearestNeighbor
{
public:
	ScalingGeneratorNearestNeighbor(const T_Source *source) :
		_srcWrapper(source)
	{}
	
	Pixel at(const Vec2D &p)
	{
		return _srcWrapper.pixel(p.toQPoint());
	}
	
private:
	
	SourceWrapper<T_Source, T_SpreadType> _srcWrapper;
};

template <class T_Source, Malachite::SpreadType T_SpreadType>
class ScalingGeneratorBilinear
{
public:
	ScalingGeneratorBilinear(const T_Source *source) :
		_srcWrapper(source)
	{}
	
	Pixel at(const Vec2D &p)
	{
		Vec2D rp(round(p.x()), round(p.y()));
		Vec2D d = p - rp + Vec2D(0.5, 0.5);
		float dx = d.x();
		float dy = d.y();
		QPoint irp = rp.toQPoint();
		
		Pixel c11, c12, c21, c22;
		
		c11 = _srcWrapper.pixel(irp + QPoint(-1, -1));
		c12 = _srcWrapper.pixel(irp + QPoint(-1, 0));
		c21 = _srcWrapper.pixel(irp + QPoint(0, -1));
		c22 = _srcWrapper.pixel(irp);
		
		Pixel result;
		result.rv() = (c11.v() * (1.f - dx) + c21.v() * dx) * (1.f - dy) + (c12.v() * (1.f - dx) + c22.v() * dx) * dy;
		
		return result;
	}
	
private:
	
	SourceWrapper<T_Source, T_SpreadType> _srcWrapper;
};

template <class T_Source, Malachite::SpreadType T_SpreadType, class T_WeightMethod>
class ScalingGenerator2
{
public:
	ScalingGenerator2(const T_Source *source) :
		_srcWrapper(source)
	{}
	
	Pixel at(const Vec2D &p)
	{
		reset(p);
		
		QPoint fp(round(p.x()), round(p.y()));
		
		addPixels(fp);
		
		Pixel result = _argb;
		
		if (_divisor == 0)
		{
			result = Pixel(0);
		}
		else
		{
			result.rv() /= _divisor;
			result.rv() = result.v().bound(PixelVec(0), PixelVec(1));
		}
		
		return result;
	}
	
private:
	
	void addPixels(const QPoint &p)
	{
		addPixel(p + QPoint(-2, -2));
		addPixel(p + QPoint(1, -2));
		addPixel(p + QPoint(-2, 1));
		addPixel(p + QPoint(1, 1));
		
		addPixel(p + QPoint(-1, -2));
		addPixel(p + QPoint(0, -2));
		addPixel(p + QPoint(-2, -1));
		addPixel(p + QPoint(1, -1));
		addPixel(p + QPoint(-2, 0));
		addPixel(p + QPoint(1, 0));
		addPixel(p + QPoint(-1, 1));
		addPixel(p + QPoint(0, 1));
		
		addPixel(p + QPoint(-1, -1));
		addPixel(p + QPoint(0, -1));
		addPixel(p + QPoint(-1, 0));
		addPixel(p + QPoint(0, 0));
	}
	
	void addPixel(const QPoint &p)
	{
		float w = T_WeightMethod::weight(Vec2D(p) + Vec2D(0.5, 0.5) - _p);
		_divisor += w;
		_argb.rv() += _srcWrapper.pixel(p).v() * w;
	}
	
	void reset()
	{
		_divisor = 0;
		_argb = Pixel(0);
	}
	
	void reset(const Vec2D &p)
	{
		reset();
		_p = p;
	}

	Vec2D _p;
	float _divisor;
	Pixel _argb;
	SourceWrapper<T_Source, T_SpreadType> _srcWrapper;
};

class ScalingWeightMethodBicubic
{
public:
	static double weight(const Vec2D &d)
	{
		double dx = fabs(d.x());
		double dy = fabs(d.y());
		
		double wx = dx <= 1.0 ? f01(dx) : f12(dx);
		double wy = dy <= 1.0 ? f01(dy) : f12(dy);
		return wx * wy;
	}
	
private:
	
	static double f01(double x)
	{
		//return x*x*x - 2.0*x*x + 1.0;
		return x*x * (x - 2.0) + 1.0;
	}
	
	static double f12(double x)
	{
		//return -x*x*x + 5.0*x*x - 8*x + 4;
		return x * (x * (-x + 5.0) - 8.0) + 4.0;
	}
};

class ScalingWeightMethodLanczos2
{
public:
	static double weight(const Vec2D &d)
	{
		Vec2D pd = M_PI * d;
		Vec2D hpd = 0.5 * M_PI * d;
		Vec2D dd = d * d;
		
		return sin(pd.x()) * sin(hpd .x()) * sin(pd.y()) * sin(hpd.y()) / (0.25 * M_PI * M_PI * M_PI * M_PI * dd.x() * dd.y());
	}
};

class ScalingWeightMethodLanczos2Hypot
{
public:
	static double weight(const Vec2D &d)
	{
		double dist = d.length();
		return sin(M_PI * dist) * sin(0.5 * M_PI * dist) / (0.5 * M_PI * M_PI * dist * dist);
	}
};

}
