#pragma once

#include <QPoint>
#include <QVector>
#include <Malachite/Vec2D>
#include <Malachite/Pixel>

namespace PaintField {

struct BrushScanline
{
	QPoint pos;
	int count;	//negative: solid
	const float *covers;
};

class BrushRasterizer
{
public:
	
	/**
	 *
	 * @param center
	 * @param radius
	 * @param aaWidth
	 * @param coverData size must be more than boundingRect().width() / 4 + 1
	 */
	BrushRasterizer(const Malachite::Vec2D &center, float radius, float aaWidth, Malachite::PixelVec *coverData);
	
	BrushScanline nextScanline();
	bool hasNextScanline() { return _y <= _rect.bottom(); }
	
	QRect boundingRect() const { return _rect; }
	
private:
	
	QRect _rect;
	int _y;
	Malachite::PixelVec _offsetCenterXs, _offsetCenterYs, _cutoffSlopes, _radiuses, _maxs;
	float _max;
	Malachite::PixelVec *_covers;
};


} // namespace PaintField

