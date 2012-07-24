#ifndef FSDOUBLESLIDER_H
#define FSDOUBLESLIDER_H

#include <QSlider>

class FSDoubleSlider : public QSlider
{
	Q_OBJECT
public:
	explicit FSDoubleSlider(QWidget *parent = 0);
	
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
	
	void notifyRangeChanged();
	void notifyIntValueChanged(int x);
	
private:
	
	double _doubleValue, _doubleMax, _doubleMin;
};

#endif // FSDOUBLESLIDER_H
