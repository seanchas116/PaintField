
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

BrushRasterizerFast::BrushRasterizerFast(const Vec2D &center, float radius, float aaWidth, PixelVec *coverData)
{
	_rect = QRectF(center.x() - radius, center.y() - radius, radius * 2.f, radius * 2.f).toAlignedRect();
	_y = _rect.top();
	
	float max, cutoffSlope;
	
	if (radius <= 1.f)
	{
		max = radius;
		//cutoff = 0.f;
		radius = 1.f;
		cutoffSlope = -max;
	}
	else if (radius <= 1.f + aaWidth)
	{
		max = 1.f;
		//cutoff = 0.f;
		cutoffSlope = -max / radius;
	}
	else
	{
		max = 1.f;
		//cutoff = radius - aaWidth;
		cutoffSlope = -max / aaWidth;
	}
	
	_max = max;
	_maxs = PixelVec(max);
	_radiuses = PixelVec(radius);
	_cutoffSlopes = PixelVec(cutoffSlope);
	_offsetCenterXs = PixelVec(center.x() - 0.5f);
	_offsetCenterYs = PixelVec(center.y() - 0.5f);
	
	_covers = coverData;
}

inline static PixelVec sseVec4FromInt(int x1, int x2, int x3, int x4)
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

inline static PixelVec sseVec4FromInt(int32_t i)
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
	
	PixelVec *vp = _covers;
	
	BrushScanline scanline;
	scanline.pos = QPoint(x, _y);
	scanline.count = _rect.width();
	scanline.covers = reinterpret_cast<float *>(vp);
	
	PixelVec xs, ys;
	xs = sseVec4FromInt(x, x+1, x+2, x+3) - _offsetCenterXs;
	ys = sseVec4FromInt(_y) - _offsetCenterYs;
	
	PixelVec yys = ys * ys;
	PixelVec zeros(0.f);
	PixelVec fours(4.f);
	
	for (int i = 0; i < scanline.count; i += 4)
	{
		PixelVec rrs = xs * xs + yys;
		PixelVec rs = sseRsqrt(rrs) * rrs;
		
		PixelVec covers = ((rs - _radiuses) * _cutoffSlopes).bound(zeros, _maxs);
		
		for (int iv = 0; iv < 4; ++iv)
			if (isnan(covers[iv]))
				covers[iv] = _max;
		
		*vp = covers;
		vp++;
		
		xs += fours;
	}
	
	++_y;
	return scanline;
}


} // namespace PaintField
