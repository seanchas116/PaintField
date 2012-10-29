#ifndef FSLOOSESPINBOX_H
#define FSLOOSESPINBOX_H

#include <QWidget>
#include <QDoubleSpinBox>

namespace PaintField
{

class LooseSpinBox : public QAbstractSpinBox
{
	Q_OBJECT
public:
	explicit LooseSpinBox(QWidget *parent = 0);
	
	void stepBy(int steps);
	
	void setDecimals(int decimals) { _decimals = decimals; updateText(); }
	int decimals() const { return _decimals; }
	
	double value() const { return _value; }
	
	void setMaximum(double max) { _max = max; updateText(); }
	double maximum() const { return _max; }
	
	void setMinimum(double min) { _min = min; updateText(); }
	double minimum() const { return _min; }
	
	void setSingleStep(double singleStep) { _singleStep = singleStep; }
	double singleStep() const { return _singleStep; }
	
signals:
	
	void valueChanged(double value);
	
public slots:
	
	void setValue(double value);
	void setText(const QString &text);
	
protected:
	
	StepEnabled stepEnabled() const;
	
private slots:
	
	void onEditingFinished();
	void updateText();
	
private:
	
	int _decimals;
	double _value, _min, _max, _singleStep;
};

}

#endif // FSLOOSESPINBOX_H
