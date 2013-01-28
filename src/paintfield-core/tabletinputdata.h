#ifndef FSTABLETINPUTDATA_H
#define FSTABLETINPUTDATA_H

#include <Malachite/Vec2D>
#include <QTransform>

namespace PaintField {

struct TabletInputData
{
	TabletInputData()
	{}
	
	TabletInputData(const Malachite::Vec2D &pos, double pressure, double rotation, double tangentialPressure, const Malachite::Vec2D &tilt) :
		pos(pos),
		tilt(tilt),
		pressure(pressure),
		rotation(rotation),
		tangentialPressure(tangentialPressure)
	{}
	
	Malachite::Vec2D pos, tilt;
	double pressure, rotation, tangentialPressure;
};

}

#endif // FSTABLETINPUTDATA_H
