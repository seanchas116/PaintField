#include <QDebug>

#include "fsmodulardoublespinbox.h"

FSModularDoubleSpinBox::FSModularDoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent)
{
	setMinimum(0);
}

void FSModularDoubleSpinBox::stepBy(int steps)
{
	qDebug() << Q_FUNC_INFO;
	double v = value() + singleStep() * steps;
	setValue(fmod(v, maximum()));
}
