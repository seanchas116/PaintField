#include "brushsidebar.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QFormLayout>
#include <QCheckBox>

namespace PaintField {

BrushSideBar::BrushSideBar(QWidget *parent) :
	MVVMView(parent)
{
	auto formLayout = new QFormLayout();
	
	auto presetLabel = new QLabel();
	formLayout->addRow(tr("Preset"), presetLabel);

	{
		auto hlayout = new QHBoxLayout();
		
		auto slider = new QSlider(Qt::Horizontal);
		slider->setMaximum(200);
		slider->setMinimum(1);
		this->route(slider, "value", "size");

		hlayout->addWidget(slider, 0);

		auto spinBox = new QSpinBox;
		spinBox->setMaximum(200);
		spinBox->setMinimum(1);
		Property::sync(spinBox, "value", slider, "value");

		hlayout->addWidget(spinBox, 0);
		
		formLayout->addRow(tr("Size"), hlayout);
	}
	
	{
		auto checkBox = new QCheckBox(tr("Smooth"));
		checkBox->setToolTip(tr("Check to paint beautifully, uncheck to paint faster"));
		this->route(checkBox, "checked", "smooth");

		formLayout->addRow(checkBox);
	}
	
	setLayout(formLayout);

	auto setTitle = [this, presetLabel](const QString &rawTitle) {

		QString title;
		if (rawTitle.isEmpty())
			title = QString("[%1]").arg(tr("Not Selected"));
		else
			title = rawTitle;

		presetLabel->setText(QString("<b>%1</b>").arg(title));
	};

	this->route(customProperty(this, setTitle), "title");
}

} // namespace PaintField
