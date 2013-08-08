#include <cmath>
#include "looseslider.h"

namespace PaintField
{

LooseSlider::LooseSlider(Qt::Orientation orientation, QWidget *parent) :
	QSlider(orientation, parent),
	_doubleValue(0.0),
	_doubleMax(1.0),
	_doubleMin(0.0)
{
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(onIntValueChanged(int)));
	connect(this, SIGNAL(rangeChanged(int,int)), this, SLOT(onRangeChanged()));
	setDoubleValue(_doubleValue);
}

void LooseSlider::setDoubleMaximum(double max)
{
	if (_doubleMin >= max)
		return;
	
	_doubleMax = max;
	setDoubleValue(_doubleValue);
	emit doubleRangeChanged(_doubleMin, _doubleMax);
}

void LooseSlider::setDoubleMinimum(double min)
{
	if (_doubleMax <= min)
		return;
	
	_doubleMin = min;
	setDoubleValue(_doubleValue);
	emit doubleRangeChanged(_doubleMin, _doubleMax);
}

static int toIntValue(double doubleMin, double doubleMax, double value, int intMin, int intMax)
{
	return intMin + std::round( ( value - doubleMin ) * (intMax - intMin) / (doubleMax - doubleMin) );
}

static double toDoubleValue(int intMin, int intMax, int value, double doubleMin, double doubleMax)
{
	return doubleMin + (value - intMin) * (doubleMax - doubleMin) / (intMax - intMin);
}

void LooseSlider::setDoubleValue(double x)
{
	if (_doubleValue != x)
	{
		_doubleValue = x;
		
		x = qBound(_doubleMin, x, _doubleMax);
		
		int i = toIntValue(_doubleMin, _doubleMax, x, minimum(), maximum());
		setValue(i);
		
		emit doubleValueChanged(x);
	}
}

void LooseSlider::onRangeChanged()
{
	setDoubleValue(_doubleValue);
}

void LooseSlider::onIntValueChanged(int x)
{
	if (x == toIntValue(_doubleValue, _doubleMin, _doubleValue, minimum(), maximum()))
		return;
	
	_doubleValue = toDoubleValue(minimum(), maximum(), x, _doubleMin, _doubleMax);
	
	emit doubleValueChanged(_doubleValue);
}

}
