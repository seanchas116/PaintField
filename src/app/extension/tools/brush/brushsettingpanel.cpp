#include <QtGui>
#include "loosespinbox.h"
#include "doubleslider.h"
#include "brushtool.h"
#include "application.h"

#include "brushsettingpanel.h"


namespace PaintField
{

BrushSettingPanel::BrushSettingPanel(QWidget *parent) :
    QWidget(parent),
    _setting(0)
{
	BrushToolFactory *factory = qobject_cast<BrushToolFactory *>(fsToolManager()->findToolFactory("brush"));
	Q_ASSERT(factory);
	
	_setting = factory->setting();
	
	QFormLayout *layout = new QFormLayout();
	
	DoubleSlider *slider = new DoubleSlider();
	slider->setOrientation(Qt::Horizontal);
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
	applyMacSmallSize();
}

QSize BrushSettingPanel::sizeHint() const
{
	return QSize(DefaultWidth, 1);
}

void BrushSettingPanel::onDiameterValueChanged(double value)
{
	_setting->diameter = value;
}

}


