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
	QVector<float> covers;
};

class BrushRasterizer
{
public:
	
	BrushRasterizer(const Malachite::Vec2D &center, double radius, double aaWidth);
	
	BrushScanline nextScanline();
	bool hasNextScanline();
	
	QRect boundingRect() const { return _rect; }
	
private:
	
	QRect _rect;
	int _y;
	Malachite::Vec2D _offsetCenter;
	double _radius, _cutoff, _max;
};

class BrushRasterizerThin
{
public:
	
	BrushRasterizerThin(const Malachite::Vec2D &center, double radius);
	
private:
	
	QRect _rect;
	int _y;
};

} // namespace PaintField

#endif // PAINTFIELD_BRUSHRASTERIZER_H
