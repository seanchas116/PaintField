#include "mlintdivision.h"
#include "fsdoubleedit.h"

FSDoubleEdit::FSDoubleEdit(QWidget *parent) :
	QLineEdit(parent),
	_value(0.0),
	_unit(1.0)
{
	connect(this, SIGNAL(editingFinished()), this, SLOT(notifyEditingFinished()));
	setValue(_value);
}

void FSDoubleEdit::setValue(double x)
{
	if (x == _value)
		return;
	
	_value = x;
	setText(QString::number(mlAlign(x, _unit)));
	emit valueChanged(x);
}

void FSDoubleEdit::notifyEditingFinished()
{
	bool ok;
	double x = text().toDouble(&ok);
	
	if (ok)
		setValue(x);
}



