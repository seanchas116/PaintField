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
		MLPolygon polygon;
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

double FSBrushStroker::drawInterval(const MLPolygon &polygon, const FSTabletInputData &dataStart, const FSTabletInputData &dataEnd, double carryOver)
{
	int count = polygon.size() - 1;
	if (count < 1)
		return carryOver;
	
	QVector<double> lengths;
	double totalLength = 0;
	
	lengths.reserve(count);
	
	for (int i = 0; i < count; ++i) {
		MLVec2D delta = polygon.at(i+1) - polygon.at(i);
		double length = mlLength(delta);
		totalLength += length;
		lengths << length;
	}
	
	double totalNormalizeFactor = 1.0 / totalLength;
	
	double pressureNDelta = (dataEnd.pressure - dataStart.pressure) * totalNormalizeFactor;
	double rotationNDelta = (dataEnd.rotation - dataStart.rotation) * totalNormalizeFactor;
	double tangentialPressureNDelta = (dataEnd.tangentialPressure - dataStart.tangentialPressure) * totalNormalizeFactor;
	MLVec2D tiltNDelta = (dataEnd.tilt - dataStart.tilt) * totalNormalizeFactor;
	
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
		MLVec2D nDelta = (polygon.at(i+1) - polygon.at(i)) / length;
		
		data.pos += nDelta * carryOver;
		data.pressure += pressureNDelta;
		data.rotation += rotationNDelta;
		data.tangentialPressure += tangentialPressureNDelta;
		data.tilt += tiltNDelta;
		
		drawDab(data);
		
		double remainingLength = length - carryOver;
		int dabCount = ceil(remainingLength) - 1;
		
		for (int j = 0; j < dabCount; ++j)
		{
			data.pos += nDelta;
			data.pressure += pressureNDelta;
			data.rotation += rotationNDelta;
			data.tangentialPressure += tangentialPressureNDelta;
			data.tilt += tiltNDelta;
			
			drawDab(data);
		}
		
		carryOver = 1 - (remainingLength - dabCount);
	}
	
	return carryOver;
}

void FSBrushStroker::drawDab(const FSTabletInputData &data)
{
	//qDebug() << "drawing dab" << data.pos.x << data.pos.y;
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
