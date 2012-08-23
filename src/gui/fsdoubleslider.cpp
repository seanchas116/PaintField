#include "fsdoubleslider.h"

FSDoubleSlider::FSDoubleSlider(QWidget *parent) :
	QSlider(parent),
	_doubleValue(0.0),
	_doubleMax(1.0),
	_doubleMin(0.0)
{
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(onIntValueChanged(int)));
	connect(this, SIGNAL(rangeChanged(int,int)), this, SLOT(onRangeChanged()));
	setDoubleValue(_doubleValue);
}

void FSDoubleSlider::setDoubleMaximum(double max)
{
	if (_doubleMin >= max)
		return;
	
	_doubleMax = max;
	setDoubleValue(_doubleValue);
	emit doubleRangeChanged(_doubleMin, _doubleMax);
}

void FSDoubleSlider::setDoubleMinimum(double min)
{
	if (_doubleMax <= min)
		return;
	
	_doubleMin = min;
	setDoubleValue(_doubleValue);
	emit doubleRangeChanged(_doubleMin, _doubleMax);
}

void FSDoubleSlider::setDoubleValue(double x)
{
	x = qBound(_doubleMin, x, _doubleMax);
	
	int i = minimum() + qRound((double)(maximum() - minimum()) / (_doubleMax - _doubleMin) * (x - _doubleMin));
	
	if (i == value())
		return;
	
	setValue(i);
	
	_doubleValue = x;
	emit doubleValueChanged(_doubleValue);
}

void FSDoubleSlider::onRangeChanged()
{
	setDoubleValue(_doubleValue);
}

void FSDoubleSlider::onIntValueChanged(int x)
{
	_doubleValue = _doubleMin + (_doubleMax - _doubleMin) / (double)(maximum() - minimum()) * (double)(x - minimum());
	
	emit doubleValueChanged(_doubleValue);
}

