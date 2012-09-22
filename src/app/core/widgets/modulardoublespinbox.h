#ifndef FSMODULARDOUBLESPINBOX_H
#define FSMODULARDOUBLESPINBOX_H

#include <QDoubleSpinBox>

namespace PaintField
{

class ModularDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT
public:
	explicit ModularDoubleSpinBox(QWidget *parent = 0);
	
	void stepBy(int steps);
	
signals:
	
public slots:
	
};

}

#endif // FSMODULARDOUBLESPINBOX_H
