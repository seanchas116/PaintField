#include <QtGui>
#include <Malachite/CurveSubdivision>

#include "paintfield-core/debug.h"
#include "paintfield-core/application.h"

#include "stroker.h"

using namespace Malachite;

namespace PaintField
{

Stroker::Stroker(Surface *surface, const BrushSetting *setting, const Vec4F &argb) :
	_surface(surface),
	_originalSurface(*surface),
	_setting(setting),
	_argb(argb)
{}

void Stroker::moveTo(const TabletInputData &data)
{
	clearLastEditedKeys();
	
	_count = 1;
	_dataEnd = data;
}

void Stroker::lineTo(const TabletInputData &data)
{
	_count++;
	
	if (_count == 4)
		drawFirst(_dataStart);
	
	if (_count > 3)
	{
		Polygon polygon = CurveSubdivision(Curve4::fromCatmullRom(_dataPrev.pos, _dataStart.pos, _dataEnd.pos, data.pos)).polygon();
		
		TabletInputData start = _dataStart;
		TabletInputData end = _dataEnd;
		
		// calculating moving average
		start.pressure = (_dataPrev.pressure + _dataStart.pressure + _dataEnd.pressure) / 3;
		end.pressure = (_dataStart.pressure + _dataEnd.pressure + data.pressure) / 3;
		
		drawInterval(polygon, start, end);
		//drawInterval(polygon, _dataStart, _dataEnd);
	}
	
	if (_count > 2)
		_dataPrev = _dataStart;
	
	_dataStart = _dataEnd;
	_dataEnd = data;
}

void Stroker::end()
{
}

void Stroker::addEditedKeys(const QPointHashToQRect &keysWithRects)
{
	for (auto iter = keysWithRects.begin(); iter != keysWithRects.end(); ++iter)
	{
		QRect rect = iter.value() | _lastEditedKeysWithRects.value(iter.key(), QRect());
		_lastEditedKeysWithRects[iter.key()] = rect;
		_totalEditedKeys << iter.key();
	}
}

QVector<double> Stroker::calcLength(const Polygon &polygon, double *totalLength)
{
	double total = 0;
	
	int count = polygon.size() - 1;
	if (count < 0)
		return QVector<double>();
	
	QVector<double> lengths(count);
	
	for (int i = 0; i < count; ++i)
	{
		double length = vecLength(polygon.at(i+1) - polygon.at(i));
		total += length;
		lengths[i] = length;
	}
	
	*totalLength = total;
	return lengths;
}

}

