#ifndef FSTABLETINPUTDATA_H
#define FSTABLETINPUTDATA_H

#include "mlvector.h"
#include <QTransform>

struct FSTabletInputData
{
	FSTabletInputData()
	{}
	
	FSTabletInputData(const MLVec2D &pos, double pressure, double rotation, double tangentialPressure, const MLVec2D &tilt) :
		pos(pos),
		tilt(tilt),
		pressure(pressure),
		rotation(rotation),
		tangentialPressure(tangentialPressure)
	{}
	
	MLVec2D pos, tilt;
	double pressure, rotation, tangentialPressure;
};

/*
inline FSTabletInputData operator+(const FSTabletInputData &data1, const FSTabletInputData &data2)
{
	FSTabletInputData result;
	result.pos = data1.pos + data2.pos;
	result.pressure = data1.pressure + data2.pressure;
	result.rotation = data1.rotation + data2.rotation;
	result.tangentialPressure = data1.tangentialPressure + data2.tangentialPressure;
	result.xTilt = data1.xTilt + data2.xTilt;
	result.yTilt = data1.yTilt + data2.yTilt;
	return result;
}

inline FSTabletInputData operator-(const FSTabletInputData &data1, const FSTabletInputData &data2)
{
	FSTabletInputData result;
	result.pos = data1.pos - data2.pos;
	result.pressure = data1.pressure - data2.pressure;
	result.rotation = data1.rotation - data2.rotation;
	result.tangentialPressure = data1.tangentialPressure - data2.tangentialPressure;
	result.xTilt = data1.xTilt - data2.xTilt;
	result.yTilt = data1.yTilt - data2.yTilt;
	return result;
}

inline FSTabletInputData operator*(const FSTabletInputData &data, double factor)
{
	FSTabletInputData result = data;
	result.pos *= factor;
	result.pressure *= factor;
	result.rotation *= factor;
	result.tangentialPressure *= factor;
	result.xTilt *= factor;
	result.yTilt *= factor;
	return result;
}

inline FSTabletInputData operator/(const FSTabletInputData &data, double divisor)
{
	FSTabletInputData result = data;
	result.pos /= divisor;
	result.pressure /= divisor;
	result.rotation /= divisor;
	result.tangentialPressure /= divisor;
	result.xTilt /= divisor;
	result.yTilt /= divisor;
	return result;
}*/

#endif // FSTABLETINPUTDATA_H
