#include <cmath>
#include "doubleslider.h"

namespace PaintField
{

DoubleSlider::DoubleSlider(Qt::Orientation orientation, QWidget *parent) :
	QSlider(orientation, parent),
	_doubleValue(0.0),
	_doubleMax(1.0),
	_doubleMin(0.0)
{
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(onIntValueChanged(int)));
	connect(this, SIGNAL(rangeChanged(int,int)), this, SLOT(onRangeChanged()));
	setDoubleValue(_doubleValue);
}

void DoubleSlider::setDoubleMaximum(double max)
{
	if (_doubleMin >= max)
		return;
	
	_doubleMax = max;
	setDoubleValue(_doubleValue);
	emit doubleRangeChanged(_doubleMin, _doubleMax);
}

void DoubleSlider::setDoubleMinimum(double min)
{
	if (_doubleMax <= min)
		return;
	
	_doubleMin = min;
	setDoubleValue(_doubleValue);
	emit doubleRangeChanged(_doubleMin, _doubleMax);
}

void DoubleSlider::setDoubleValue(double x)
{
	if (_doubleValue != x)
	{
		_doubleValue = x;
		
		x = qBound(_doubleMin, x, _doubleMax);
		
		int i = minimum() + ::round((x - _doubleMin) * double(maximum() - minimum()) / (_doubleMax - _doubleMin));
		setValue(i);
		
		emit doubleValueChanged(x);
	}
}

void DoubleSlider::onRangeChanged()
{
	setDoubleValue(_doubleValue);
}

void DoubleSlider::onIntValueChanged(int x)
{
	_doubleValue = double(x - minimum()) * (_doubleMax - _doubleMin) / double(maximum() - minimum());
	
	emit doubleValueChanged(_doubleValue);
}

}
