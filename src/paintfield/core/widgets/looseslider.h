#pragma once

#include <QSlider>
#include "../global.h"

namespace PaintField
{

class LooseSlider : public QSlider
{
	Q_OBJECT
public:
	LooseSlider(Qt::Orientation orientation, QWidget *parent = 0);
	~LooseSlider();
	
	void setDecimals(int decimals);
	void setDoubleRange(double min, double max);

	int decimals() const;
	double doubleMaximum() const;
	double doubleMinimum() const;
	double doubleValue() const;
	
signals:
	
	void doubleValueChanged(double x);
	
public slots:
	
	void setDoubleValue(double x);
	
private slots:
	
	void onIntValueChanged(int x);
	
private:
	
	struct Data;
	QScopedPointer<Data> d;
};

}

