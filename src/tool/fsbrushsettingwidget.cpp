#include <QtGui>
#include "fsloosespinbox.h"
#include "fsdoubleslider.h"
#include "fsbrushtool.h"
#include "fscore.h"

#include "fsbrushsettingwidget.h"


FSBrushSettingWidget::FSBrushSettingWidget(QWidget *parent) :
    FSPanelWidget(parent),
    _setting(0)
{
	FSBrushToolFactory *factory = qobject_cast<FSBrushToolFactory *>(fsToolManager()->findToolFactory("brush"));
	Q_ASSERT(factory);
	
	_setting = factory->setting();
	
	QFormLayout *layout = new QFormLayout();
	
	FSDoubleSlider *slider = new FSDoubleSlider();
	slider->setOrientation(Qt::Horizontal);
	slider->setMaximum(1000);
	slider->setMinimum(1);
	slider->setDoubleMaximum(100.0);
	slider->setDoubleMinimum(0.1);
	slider->setDoubleValue(1);
	
	FSLooseSpinBox *spinBox = new FSLooseSpinBox();
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

QSize FSBrushSettingWidget::sizeHint() const
{
	return QSize(DefaultWidth, 1);
}

void FSBrushSettingWidget::onDiameterValueChanged(double value)
{
	_setting->diameter = value;
}



