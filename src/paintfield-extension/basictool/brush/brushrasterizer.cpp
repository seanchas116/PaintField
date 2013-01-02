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
}

BrushScanline BrushRasterizer::nextScanline()
{
	BrushScanline scanline;
	
	scanline.pos = QPoint(_rect.left(), _y);
	scanline.count = _rect.width();
	scanline.covers.resize(scanline.count);
	
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
		
		scanline.covers[i] = cover;
	}
	
	++_y;
	return scanline;
}

bool BrushRasterizer::hasNextScanline()
{
	return _y <= _rect.bottom();
}


} // namespace PaintField
