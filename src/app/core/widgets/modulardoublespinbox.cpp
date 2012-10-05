#include <QDebug>

#include "modulardoublespinbox.h"

namespace PaintField
{

ModularDoubleSpinBox::ModularDoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent)
{
	setMinimum(0);
}

void ModularDoubleSpinBox::stepBy(int steps)
{
	qDebug() << Q_FUNC_INFO;
	double v = value() + singleStep() * steps;
	setValue(fmod(v, maximum()));
}

}
