
#include "brushrasterizer.h"

namespace PaintField {

using namespace std;
using namespace Malachite;

BrushRasterizer::BrushRasterizer(const Vec2D &center, double radius, double aaWidth)
{
	_rect = QRectF(center.x() - radius, center.y() - radius, radius * 2, radius * 2).toAlignedRect();
	_y = _rect.top();
	_offsetCenter = center - Vec2D(0.5);
	
	if (radius <= 1.0)
	{
		_max = radius;
		_cutoff = 0.0;
		_radius = 1.0;
	}
	else if (radius <= 1.0 + aaWidth)
	{
		_max = 1.0;
		_cutoff = 0.0;
		_radius = radius;
	}
	else
	{
		_max = 1.0;
		_cutoff = radius - aaWidth;
		_radius = radius;
	}
	
	_covers.reset(new float[_rect.width()]);
}

BrushScanline BrushRasterizer::nextScanline()
{
	BrushScanline scanline;
	
	scanline.pos = QPoint(_rect.left(), _y);
	scanline.count = _rect.width();
	scanline.covers = _covers.data();
	
	for (int i = 0; i < scanline.count; ++i)
	{
		double cover;
		
		Vec2D pos(_rect.left() + i, _y);
		Vec2D dp = pos - _offsetCenter;
		
		if (dp.x() == 0 && dp.y() == 0)
		{
			cover = _max;
		}
		else
		{
			double r = dp.length();
			
			if (r <= _cutoff)
				cover = _max;
			else if (r < _radius)
				cover = _max * (_radius - r) / (_radius - _cutoff);
			else
				cover = 0;
		}
		
		_covers[i] = cover;
	}
	
	++_y;
	return scanline;
}

BrushRasterizerFast::BrushRasterizerFast(const Vec2D &center, float radius, float aaWidth)
{
	_rect = QRectF(center.x() - radius, center.y() - radius, radius * 2.f, radius * 2.f).toAlignedRect();
	_y = _rect.top();
	
	if (radius <= 1.f)
	{
		_max = radius;
		_cutoff = 0.f;
		_radius = 1.f;
	}
	else if (radius <= 1.f + aaWidth)
	{
		_max = 1.f;
		_cutoff = 0.f;
		_radius = radius;
	}
	else
	{
		_max = 1.f;
		_cutoff = radius - aaWidth;
		_radius = radius;
	}
	
	_cutoffSlope = _max / (_cutoff - _radius);
	
	int coverCount = (_rect.width() / 4 + 1) * 4;
	_covers.reset(new float[coverCount]);
	
	_offsetCenterXs = Vector<float, 4>(center.x() - 0.5f);
	_offsetCenterYs = Vector<float, 4>(center.y() - 0.5f);
}

inline static Vector<float, 4> sseVec4FromInt(int x1, int x2, int x3, int x4)
{
	union
	{
		std::array<int32_t, 4> a;
		__m128i m;
	} u;
	
	u.a[0] = x1;
	u.a[1] = x2;
	u.a[2] = x3;
	u.a[3] = x4;
	return _mm_cvtepi32_ps(u.m);
}

inline static Vector<float, 4> sseVec4FromInt(int32_t i)
{
	union
	{
		std::array<int32_t, 4> a;
		__m128i m;
	} u;
	
	u.a[0] = i;
	u.m = _mm_unpacklo_epi32(u.m, u.m);
	u.m = _mm_unpacklo_epi32(u.m, u.m);
	return _mm_cvtepi32_ps(u.m);
}

BrushScanline BrushRasterizerFast::nextScanline()
{
	int x = _rect.left();
	
	BrushScanline scanline;
	scanline.pos = QPoint(x, _y);
	scanline.count = _rect.width();
	scanline.covers = _covers.data();
	
	float *p = _covers.data();
	
	PixelVec xs, ys;
	xs = sseVec4FromInt(x, x+1, x+2, x+3);
	ys = sseVec4FromInt(_y);
	
	xs -= _offsetCenterXs;
	ys -= _offsetCenterYs;
	
	PixelVec yys = ys * ys;
	
	for (int i = 0; i < scanline.count; i += 4, xs += PixelVec(4.f))
	{
		PixelVec rrs = xs * xs + yys;
		PixelVec rs = sseRsqrt(rrs) * rrs;
		
		for (int iv = 0; iv < 4; ++iv)
		{
			float r = rs[iv];
			float cover;
			
			if (r <= _cutoff)
				cover = _max;
			else if (r < _radius)
				cover = (r - _radius) * _cutoffSlope;
			else if (isnan(r))
				cover = _max;
			else
				cover = 0;
			
			*p = cover;
			++p;
		}
	}
	
	++_y;
	return scanline;
}



} // namespace PaintField
