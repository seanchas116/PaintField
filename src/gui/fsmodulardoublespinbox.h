#ifndef FSMODULARDOUBLESPINBOX_H
#define FSMODULARDOUBLESPINBOX_H

#include <QDoubleSpinBox>

class FSModularDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT
public:
	explicit FSModularDoubleSpinBox(QWidget *parent = 0);
	
	void stepBy(int steps);
	
signals:
	
public slots:
	
};

#endif // FSMODULARDOUBLESPINBOX_H
