
#include <Malachite/CurveSubdivision>

#include "brushstroker.h"

using namespace Malachite;

namespace PaintField {

BrushStroker::BrushStroker(Surface *surface) :
	_surface(surface),
	_originalSurface(*surface)
{}

void BrushStroker::moveTo(const TabletInputData &data)
{
	clearLastEditedKeys();
	
	_count = 1;
	_dataEnd = data;
}

void BrushStroker::lineTo(const TabletInputData &data)
{
	_count++;
	
	if (_count == 4)
		drawFirst(_dataStart);
	
	if (_count > 3)
	{
		Polygon polygon = CurveSubdivision(Curve4::fromBSpline(_dataPrev.pos, _dataStart.pos, _dataEnd.pos, data.pos)).polygon();
		
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

void BrushStroker::end()
{
}

void BrushStroker::addEditedKeys(const QHash<QPoint, QRect> &keysWithRects)
{
	for (auto iter = keysWithRects.begin(); iter != keysWithRects.end(); ++iter)
		addEditedKey(iter.key(), iter.value());
}

void BrushStroker::addEditedKey(const QPoint &key, const QRect &rect)
{
	_lastEditedKeysWithRects[key] |= rect;
	_totalEditedKeys << key;
}

void BrushStroker::addEditedRect(const QRect &rect)
{
	auto topLeftKey = Surface::keyForPixel(rect.topLeft());
	auto bottomRightKey = Surface::keyForPixel(rect.bottomRight());
	
	for (int y = topLeftKey.y(); y <= bottomRightKey.y(); ++y)
	{
		for (int x = topLeftKey.x(); x <= bottomRightKey.x(); ++x)
		{
			QPoint key(x, y);
			auto keyRect = rect.translated(key * -Surface::tileWidth()) & Surface::keyToRect(0, 0);
			addEditedKey(key, keyRect);
		}
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
		double length = (polygon.at(i+1) - polygon.at(i)).length();
		total += length;
		lengths[i] = length;
	}
	
	*totalLength = total;
	return lengths;
}

}

