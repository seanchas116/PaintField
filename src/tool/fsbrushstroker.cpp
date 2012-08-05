#include <QtGui>
#include "fsscopedtimer.h"
#include "mlcurvesubdivision.h"

#include "fsbrushstroker.h"

FSBrushStroker::FSBrushStroker(MLSurface *surface) :
	_surface(surface),
	_radius(5)
{
}

void FSBrushStroker::moveTo(const FSTabletInputData &data)
{
	_lastEditedKeys.clear();
	_stackCount = 1;
	_dataEnd = data;
	_carryOver = 0;
}

void FSBrushStroker::lineTo(const FSTabletInputData &data)
{
	FSScopedTimer timer(__PRETTY_FUNCTION__);
	
	_lastEditedKeys.clear();
	
	Q_ASSERT(1 <= _stackCount && _stackCount <= 3);
	
	if (_stackCount != 1) {
		QPolygonF polygon;
		if (_stackCount == 3)
			polygon = MLCurveSubdivision(MLCurve4::fromCatmullRom(_posPrevious, _dataStart.pos, _dataEnd.pos, data.pos)).polygon();
		
		/*
		if (_stackCount == 2)
			polygon = FSCurveSubdivision(FSCurve4::fromCatmullRomStart(_dataStart.pos, _dataEnd.pos, data.pos)).polygon();
		else	// _stackCount == 3
			polygon = FSCurveSubdivision(FSCurve4::fromCatmullRom(_posPrevious, _dataStart.pos, _dataEnd.pos, data.pos)).polygon();
		*/
		
		_carryOver = drawInterval(polygon, _dataStart, _dataEnd, _carryOver);
	}
	
	_posPrevious = _dataStart.pos;
	_dataStart = _dataEnd;
	_dataEnd = data;
	
	if (_stackCount != 3)
		_stackCount++;
	
	_totalEditedKeys |= _lastEditedKeys;
}

void FSBrushStroker::end()
{
	
}

double FSBrushStroker::drawInterval(const QPolygonF &polygon, const FSTabletInputData &dataStart, const FSTabletInputData &dataEnd, double carryOver)
{
	int count = polygon.size() - 1;
	if (count < 1)
		return carryOver;
	
	QVector<double> lengths;
	double totalLength = 0;
	
	lengths.reserve(count);
	
	for (int i = 0; i < count; ++i) {
		QPointF delta = polygon.at(i+1) - polygon.at(i);
		double length = hypot(delta.x(), delta.y());
		totalLength += length;
		lengths << length;
	}
	
	double pressureNDelta = (dataEnd.pressure - dataStart.pressure) / totalLength;
	double rotationNDelta = (dataEnd.rotation - dataStart.rotation) / totalLength;
	double tangentialPressureNDelta = (dataEnd.tangentialPressure - dataStart.tangentialPressure) / totalLength;
	double xTiltNDelta = (dataEnd.xTilt - dataStart.xTilt) / totalLength;
	double yTiltNDelta = (dataEnd.yTilt - dataStart.yTilt) / totalLength;
	
	FSTabletInputData data = dataStart;
	
	for (int i = 0; i < count; ++i)
	{
		double length = lengths.at(i);
		
		if (!length)
			continue;
		if (carryOver >= length)
		{
			carryOver -= length;
			continue;
		}
		
		data.pos = polygon.at(i);
		QPointF nDelta = (polygon.at(i+1) - polygon.at(i)) / length;
		
		data.pos += nDelta * carryOver;
		data.pressure += pressureNDelta;
		data.rotation += rotationNDelta;
		data.tangentialPressure += tangentialPressureNDelta;
		data.xTilt += xTiltNDelta;
		data.yTilt += yTiltNDelta;
		
		drawDab(data);
		
		double remainingLength = length - carryOver;
		int dabCount = ceil(remainingLength) - 1;
		
		for (int j = 0; j < dabCount; ++j)
		{
			data.pos += nDelta;
			data.pressure += pressureNDelta;
			data.rotation += rotationNDelta;
			data.tangentialPressure += tangentialPressureNDelta;
			data.xTilt += xTiltNDelta;
			data.yTilt += yTiltNDelta;
			
			drawDab(data);
		}
		
		carryOver = 1 - (remainingLength - dabCount);
	}
	
	return carryOver;
}

void FSBrushStroker::drawDab(const FSTabletInputData &data)
{
	//qDebug() << "drawing dab pressure =" << data.pressure;
	MLSurfacePainter painter(_surface);
	painter.setColor(MLColor::black());
	painter.drawEllipse(data.pos, _radius * data.pressure, _radius * data.pressure);
	painter.flush();
	_lastEditedKeys |= painter.editedKeys();
}

void FSBrushStroker::drawSimpleDab(const QPointF &pos)
{
	MLSurfacePainter painter(_surface);
	painter.setColor(MLColor::black());
	painter.drawEllipse(pos, _radius, _radius);
	painter.flush();
	_lastEditedKeys |= painter.editedKeys();
}
