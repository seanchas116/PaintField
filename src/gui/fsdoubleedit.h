#ifndef FSDOUBLEEDIT_H
#define FSDOUBLEEDIT_H

#include <QLineEdit>

class FSDoubleEdit : public QLineEdit
{
	Q_OBJECT
public:
	explicit FSDoubleEdit(QWidget *parent = 0);
	
	double value() const { return _value; }
	double unit() const { return _unit; }
	void setUnit(double unit) { _unit = unit; }
	
signals:
	
	void valueChanged(double x);
	
public slots:
	
	void setValue(double x);
	
private slots:
	
	void notifyEditingFinished();
	
private:
	
	double _value, _unit;
};

#endif // FSDOUBLEEDIT_H
