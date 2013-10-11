#include <cmath>
#include "looseslider.h"

namespace PaintField
{

struct LooseSlider::Data
{
	double mValue = 0.0;
	double mMax = 1.0;
	double mMin = 0.0;
	double mIntFactor = 10;
	int mDecimals = 1;

	void setDecimals(int x)
	{
		mDecimals = x;
		mIntFactor = std::pow(10, x);
	}

	void updateIntRange(LooseSlider *self)
	{
		self->setRange(std::round(mMin * mIntFactor), std::round(mMax * mIntFactor));
	}

	void updateIntValue(LooseSlider *self)
	{
		self->setValue(std::round(mValue * mIntFactor));
	}
};

LooseSlider::LooseSlider(Qt::Orientation orientation, QWidget *parent) :
	QSlider(orientation, parent),
	d(new Data)
{
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(onIntValueChanged(int)));
	d->updateIntRange(this);
	d->updateIntValue(this);
}

LooseSlider::~LooseSlider()
{
}

void LooseSlider::setDecimals(int decimals)
{
	d->setDecimals(decimals);
	d->updateIntRange(this);
	d->updateIntValue(this);
}

void LooseSlider::setDoubleRange(double min, double max)
{
	d->mMin = min;
	d->mMax = max;
	d->updateIntRange(this);
	setDoubleValue(d->mValue);
}

void LooseSlider::setDoubleValue(double x)
{
	if (d->mValue != x)
	{
		x = qBound(d->mMin, x, d->mMax);
		d->mValue = x;
		d->updateIntValue(this);
		emit doubleValueChanged(x);
	}
}

int LooseSlider::decimals() const
{
	return d->mDecimals;
}

double LooseSlider::doubleMinimum() const
{
	return d->mMin;
}

double LooseSlider::doubleMaximum() const
{
	return d->mMax;
}

double LooseSlider::doubleValue() const
{
	return d->mValue;
}

void LooseSlider::onIntValueChanged(int x)
{
	if (x != std::round(d->mValue * d->mIntFactor))
		setDoubleValue(x / d->mIntFactor);
}

}
