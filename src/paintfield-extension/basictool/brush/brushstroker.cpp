#include <QtGui>
#include <Malachite/CurveSubdivision>

#include "paintfield-core/debug.h"

#include "brushstroker.h"

using namespace Malachite;

namespace PaintField {

BrushStroker::BrushStroker() :
    _mutex(QMutex::Recursive)
{}

void BrushStroker::moveTo(const TabletInput &data)
{
	clearLastEditedKeys();
	
	_count = 1;
	_dataEnd = data;
}

void BrushStroker::lineTo(const TabletInput &data)
{
	_count++;
	
	if (_count == 4)
		drawFirst(_dataStart);
	
	if (_count > 3)
	{
		Polygon polygon = CurveSubdivision(Curve4::fromBSpline(_dataPrev.pos(), _dataStart.pos(), _dataEnd.pos(), data.pos())).polygon();
		
		TabletInput start = _dataStart;
		TabletInput end = _dataEnd;
		
		// calculating moving average
		start.setPressure((_dataPrev.pressure() + _dataStart.pressure() + _dataEnd.pressure()) / 3);
		end.setPressure((_dataStart.pressure() + _dataEnd.pressure() + data.pressure()) / 3);
		
		drawInterval(polygon, start, end);
		//drawInterval(polygon, _dataStart, _dataEnd);
	}
	
	if (_count > 2)
		_dataPrev = _dataStart;
	
	_dataStart = _dataEnd;
	_dataEnd = data;
}

void BrushStroker::end()
{
}

QHash<QPoint, QRect> BrushStroker::getAndClearEditedKeysWithRects()
{
	QMutexLocker locker(&_mutex);
	
	auto result = _lastEditedKeysWithRects;
	_lastEditedKeysWithRects = QHash<QPoint, QRect>();
	return result;
}

void BrushStroker::addEditedKeys(const QHash<QPoint, QRect> &keysWithRects)
{
	QMutexLocker locker(&_mutex);
	
	for (auto iter = keysWithRects.begin(); iter != keysWithRects.end(); ++iter)
	{
		QRect rect = iter.value() | _lastEditedKeysWithRects.value(iter.key(), QRect());
		_lastEditedKeysWithRects[iter.key()] = rect;
		_totalEditedKeys << iter.key();
	}
}

QVector<double> BrushStroker::calcLength(const Polygon &polygon, double *totalLength)
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

