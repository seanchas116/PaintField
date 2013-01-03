#include "paintfield-core/debug.h"

#include "brushrasterizer.h"

namespace PaintField {

using namespace std;
using namespace Malachite;

BrushRasterizer::BrushRasterizer(const Vec2D &center, double radius, double aaWidth)
{
	_rect = QRectF(center.x - radius, center.y - radius, radius * 2, radius * 2).toAlignedRect();
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
		
		if (dp.x == 0 && dp.y == 0)
		{
			cover = _max;
		}
		else
		{
			double r = vecLength(dp);
			
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
	_rect = QRectF(center.x - radius, center.y - radius, radius * 2.f, radius * 2.f).toAlignedRect();
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
	
	_covers.reset(new float[_rect.width()]);
	
	_offsetCenterXs = Vec4F(center.x - 0.5f);
	_offsetCenterYs = Vec4F(center.y - 0.5f);
}

BrushScanline BrushRasterizerFast::nextScanline()
{
	BrushScanline scanline;
	scanline.pos = QPoint(_rect.left(), _y);
	scanline.count = _rect.width();
	scanline.covers = _covers.data();
	
	float *p = _covers.data();
	
	for (int i = 0; i < scanline.count; i += 4)
	{
		int x = _rect.left() + i;
		
		Vec4F xs, ys;
		xs = Vec4I32(x, x+1, x+2, x+3);
		ys = Vec4I32(_y);
		
		xs -= _offsetCenterXs;
		ys -= _offsetCenterYs;
		
		Vec4F rrs = xs * xs + ys * ys;
		Vec4F rs = vecRsqrt(rrs) * rrs;
		
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
