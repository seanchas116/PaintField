#include <QtGui>

#include "paintfield-core/widgets/loosespinbox.h"
#include "paintfield-core/widgets/doubleslider.h"

#include "paintfield-core/appcontroller.h"
#include "paintfield-core/util.h"

#include "brushtool.h"

#include "brushsettingsidebar.h"


namespace PaintField
{

BrushSettingSidebar::BrushSettingSidebar(BrushSetting *setting, QWidget *parent) :
    QWidget(parent),
    _setting(setting)
{
	QFormLayout *layout = new QFormLayout();
	
	DoubleSlider *slider = new DoubleSlider(Qt::Horizontal);
	slider->setMaximum(1000);
	slider->setMinimum(1);
	slider->setDoubleMaximum(100.0);
	slider->setDoubleMinimum(0.1);
	slider->setDoubleValue(1);
	
	LooseSpinBox *spinBox = new LooseSpinBox();
	spinBox->setMinimum(0.1);
	spinBox->setMaximum(100);
	
	connect(slider, SIGNAL(doubleValueChanged(double)), spinBox, SLOT(setValue(double)));
	connect(spinBox, SIGNAL(valueChanged(double)), slider, SLOT(setDoubleValue(double)));
	
	connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(onDiameterValueChanged(double)));
	spinBox->setValue(_setting->diameter);
	
	QHBoxLayout *hlayout = new QHBoxLayout();
	hlayout->addWidget(slider);
	hlayout->addWidget(spinBox);
	
	layout->addRow(tr("Width:"), hlayout);
	
	setLayout(layout);
	
	setWindowTitle(tr("Brush Settings"));
	applyMacSmallSize(this);
}

void BrushSettingSidebar::onDiameterValueChanged(double value)
{
	_setting->diameter = value;
}

}


