#ifndef FSTABLETINPUTDATA_H
#define FSTABLETINPUTDATA_H

#include <Malachite/Vector>
#include <QTransform>
#include <QSharedDataPointer>

namespace PaintField {

class TabletInputData : public QSharedData
{
public:
	
	TabletInputData() {}
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

class TabletInput
{
public:
	
	TabletInput() :
	    d(new TabletInputData())
	{}
	
	TabletInput(const Malachite::Vec2D &pos, double pressure, double rotation, double tangentialPressure, const Malachite::Vec2D &tilt) :
	    d(new TabletInputData(pos, pressure, rotation, tangentialPressure, tilt))
	{}
	
	TabletInput(const TabletInput &other) :
	    d(other.d)
	{}
	
	Malachite::Vec2D pos() const { return d->pos; }
	void setPos(const Malachite::Vec2D &pos) { d->pos = pos; }
	
	Malachite::Vec2D tilt() const { return d->tilt; }
	void setTilt(const Malachite::Vec2D &tilt) { d->tilt = tilt; }
	
	double pressure() const { return d->pressure; }
	void setPressure(double pressure) { d->pressure = pressure; }
	
	double rotation() const { return d->rotation; }
	void setRotation(double rotation) { d->rotation = rotation; }
	
	double tangentialPressure() const { return d->tangentialPressure; }
	void setTangentialPressure(double tangentialPressure) { d->tangentialPressure = tangentialPressure; }
	
private:
	
	QSharedDataPointer<TabletInputData> d;
};

}

#endif // FSTABLETINPUTDATA_H
