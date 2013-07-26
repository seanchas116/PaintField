#pragma once

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

