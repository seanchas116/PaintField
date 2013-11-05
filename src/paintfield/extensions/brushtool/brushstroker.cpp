
#include <Malachite/CurveSubdivision>

#include "brushstroker.h"

using namespace Malachite;

namespace PaintField {

BrushStroker::BrushStroker(Surface *surface) :
	_surface(surface),
	_originalSurface(*surface),
    _segment(2)
{
	//_smoothed = true;
}

void BrushStroker::moveTo(const TabletInputData &originalData)
{
	auto data = originalData;
	filterData(data);

	clearLastEditedKeys();
	
	_count = 0;
	_dataEnd = data;
	
	if (_smoothed)
	{
		_dataPrev = data;
		_dataStart = data;
	}
	else
	{
		drawFirst(data);
	}
}

void BrushStroker::lineTo(const TabletInputData &originalData)
{
	auto data = originalData;
	filterData(data);

	_count += 1;
	
	if (_smoothed)
	{
		if (_count >= 2)
		{
			auto polygon = CurveSubdivision(Curve4::fromBSpline(_dataPrev.pos, _dataStart.pos, _dataEnd.pos, data.pos)).polygon();
			
			if (_count == 2)
			{
				auto firstData = _dataStart;
				firstData.pos = polygon.first();
				drawFirst(firstData);
			}
			
			drawInterval(polygon, _dataStart, _dataEnd);
		}
		
		_dataPrev = _dataStart;
		_dataStart = _dataEnd;
		_dataEnd = data;
	}
	else
	{
		_dataStart = _dataEnd;
		_dataEnd = data;
		
		if (_dataStart.pos == _dataEnd.pos)
			return;
		
		_segment[0] = _dataStart.pos;
		_segment[1] = _dataEnd.pos;
		
		drawInterval(_segment, _dataStart, _dataEnd);
	}
}

void BrushStroker::end()
{
	if (_smoothed)
	{
		lineTo(_dataEnd);
	}
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

void BrushStroker::filterData(TabletInputData &data)
{
	data.pressure *= data.pressure;
}

QWidget *BrushStrokerFactory::createEditor(ObservableVariantMap *parameters)
{
	Q_UNUSED(parameters);
	return nullptr;
}

}

