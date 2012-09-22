#include <QtGui>

#include "mldivision.h"
#include "loosespinbox.h"

namespace PaintField
{

QString numberWithDecimals(double x, int decimals)
{
	QString stringInt = QString::number(floor(x));
	QString stringDecimal = QString::number(fmod(x, 1), 'f', decimals).mid(2);
	
	if (stringDecimal.size() > decimals)
	{
		stringDecimal = stringDecimal.left(decimals);
	}
	if (stringDecimal.size() < decimals)
	{
		stringDecimal += QString(decimals - stringDecimal.size(), '0');
	}
	
	return stringInt + "." + stringDecimal;
}

LooseSpinBox::LooseSpinBox(QWidget *parent) :
    QAbstractSpinBox(parent),
    _decimals(1),
    _value(0),
    _min(0),
    _max(1),
    _singleStep(0.1)
{
	connect(this, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
	setValue(0);
	updateText();
}

void LooseSpinBox::stepBy(int steps)
{
	setValue(_value + steps * _singleStep);
}

void LooseSpinBox::setValue(double value)
{
	if (wrapping())
	{
		value = Malachite::Division(value - _min, _max - _min).rem() + _min;
	}
	else
	{
		value = qBound(_min, value, _max);
	}
	
	if (_value == value)
		return;
	
	_value = value;
	
	updateText();
	
	emit valueChanged(value);
}

void LooseSpinBox::setText(const QString &text)
{
	bool ok;
	double x = text.toDouble(&ok);
	
	if (ok)
		setValue(x);
}

void LooseSpinBox::updateText()
{
	lineEdit()->setText(numberWithDecimals(_value, _decimals));
}

QAbstractSpinBox::StepEnabled LooseSpinBox::stepEnabled() const
{
	return StepUpEnabled | StepDownEnabled;
}

void LooseSpinBox::onEditingFinished()
{
	setText(text());
}

}
