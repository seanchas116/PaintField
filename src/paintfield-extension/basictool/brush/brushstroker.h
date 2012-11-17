#ifndef BRUSHSTROKER_H
#define BRUSHSTROKER_H

#include "stroker.h"

namespace PaintField
{

class BrushStroker : public Stroker
{
public:
	
	BrushStroker(Malachite::Surface *surface, const BrushSetting *setting, const Malachite::Vec4F &argb) : Stroker(surface, setting, argb) {}
	
protected:
	
	void drawFirst(const TabletInputData &data);
	void drawInterval(const Malachite::Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd);
	
private:
	
	void drawSegment(const Malachite::Vec2D &p1, const Malachite::Vec2D &p2, double length, TabletInputData &data, double pressureNormalized, double rotationNormalized, double tangentialPressureNormalized, const Malachite::Vec2D &tiltNormalized);
	
	void drawDab(const TabletInputData &data);
	Malachite::Image drawDabImage(const TabletInputData &data, QRect *resultRect);
	
	double _carryOver;
	double _lastMinorRadius;
};

}
#endif // BRUSHSTROKER_H
