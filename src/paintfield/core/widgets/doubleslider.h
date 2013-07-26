#pragma once

#include <QSlider>
#include "../global.h"

namespace PaintField
{

class DoubleSlider : public QSlider
{
	Q_OBJECT
public:
	DoubleSlider(Qt::Orientation orientation, QWidget *parent = 0);
	
	void setDoubleMaximum(double max);
	void setDoubleMinimum(double min);
	double doubleMaximum() const { return _doubleMax; }
	double doubleMinimum() const { return _doubleMin; }
	double doubleValue() const { return _doubleValue; }
	
signals:
	
	void doubleValueChanged(double x);
	void doubleRangeChanged(double min, double max);
	
public slots:
	
	void setDoubleValue(double x);
	
private slots:
	
	void onRangeChanged();
	void onIntValueChanged(int x);
	
private:
	
	double _doubleValue, _doubleMax, _doubleMin;
};

}

