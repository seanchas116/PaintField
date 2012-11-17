#ifndef PENSTROKER_H
#define PENSTROKER_H

#include "stroker.h"

namespace PaintField
{

class PenStroker : public Stroker
{
public:
	
	PenStroker(Malachite::Surface *surface, const BrushSetting *setting, const Malachite::Vec4F &argb) : Stroker(surface, setting, argb) {}
	
protected:
	
	void drawFirst(const TabletInputData &data);
	void drawInterval(const Malachite::Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd);
	
private:
	
	void drawShape(const Malachite::FixedMultiPolygon &shape);
	//void drawOne(const Malachite::Vec2D &pos, double pressure, bool drawQuad);
	
	static Malachite::Polygon calcTangentQuadrangle(double radius1, const Malachite::Vec2D &center1, double radius2, const Malachite::Vec2D &center2);
	
	QHash<QPoint, Malachite::FixedMultiPolygon> _drawnShapes;
	double _radiusBase;
};

}

#endif // PENSTROKER_H
