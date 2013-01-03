#ifndef PAINTFIELD_BRUSHRASTERIZER_H
#define PAINTFIELD_BRUSHRASTERIZER_H

#include <QPoint>
#include <QVector>
#include <Malachite/Vector>

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
	
	BrushRasterizer(const Malachite::Vec2D &center, double radius, double aaWidth);
	
	BrushScanline nextScanline();
	bool hasNextScanline() const { return _y <= _rect.bottom(); }
	
	QRect boundingRect() const { return _rect; }
	
private:
	
	QRect _rect;
	int _y;
	Malachite::Vec2D _offsetCenter;
	double _radius, _cutoff, _max;
	QScopedArrayPointer<float> _covers;
};

class BrushRasterizerFast
{
public:
	
	BrushRasterizerFast(const Malachite::Vec2D &center, float radius, float aaWidth);
	
	BrushScanline nextScanline();
	bool hasNextScanline() { return _y <= _rect.bottom(); }
	
	QRect boundingRect() const { return _rect; }
	
private:
	
	QRect _rect;
	int _y;
	Malachite::Vec4F _offsetCenterXs, _offsetCenterYs;
	float _radius, _cutoff, _max, _cutoffSlope;
	QScopedArrayPointer<float> _covers;
};

} // namespace PaintField

#endif // PAINTFIELD_BRUSHRASTERIZER_H
