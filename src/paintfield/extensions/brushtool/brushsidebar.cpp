#include "brushsidebar.h"

#include "brushpreferencesmanager.h"
#include "brushpresetmanager.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QFormLayout>
#include <QCheckBox>

namespace PaintField {

BrushSideBar::BrushSideBar(BrushPresetManager *presetManager, BrushPreferencesManager *prefManager, QWidget *parent) :
    QWidget(parent)
{
	auto formLayout = new QFormLayout();
	
	auto presetLabel = new QLabel();
	formLayout->addRow(tr("Preset"), presetLabel);
	
	{
		auto hlayout = new QHBoxLayout();
		
		{
			auto slider = new QSlider(Qt::Horizontal);
			slider->setMaximum(200);
			slider->setMinimum(1);
			
			slider->setValue(prefManager->brushSize());
			connect(slider, SIGNAL(valueChanged(int)), prefManager, SLOT(setBrushSize(int)));
			connect(prefManager, SIGNAL(brushSizeChanged(int)), slider, SLOT(setValue(int)));
			
			hlayout->addWidget(slider, 0);
		}
		
		{
			auto spinBox = new QSpinBox;
			spinBox->setMaximum(200);
			spinBox->setMinimum(1);
			
			spinBox->setValue(prefManager->brushSize());
			connect(spinBox, SIGNAL(valueChanged(int)), prefManager, SLOT(setBrushSize(int)));
			connect(prefManager, SIGNAL(brushSizeChanged(int)), spinBox, SLOT(setValue(int)));
			
			hlayout->addWidget(spinBox, 0);
		}
		
		formLayout->addRow(tr("Size"), hlayout);
	}
	
	{
		auto checkBox = new QCheckBox(tr("Smooth"));
		checkBox->setToolTip(tr("Check to paint beautifully, uncheck to paint faster"));
		checkBox->setChecked(prefManager->isSmoothEnabled());
		connect(checkBox, SIGNAL(toggled(bool)), prefManager, SLOT(setSmoothEnabled(bool)));
		connect(prefManager, SIGNAL(smoothEnabledChanged(bool)), checkBox, SLOT(setChecked(bool)));
		formLayout->addRow(checkBox);
	}
	
	setLayout(formLayout);

	auto setPresetMetadata = [this, presetLabel](const QVariantMap &metadata) {

		auto rawTitle = metadata["title"].toString();
		QString title;
		if (rawTitle.isEmpty())
			title = QString("[%1]").arg(tr("Not Selected"));
		else
			title = rawTitle;

		presetLabel->setText(QString("<b>%1</b>").arg(title));
	};

	connect(presetManager, &BrushPresetManager::metadataChanged, this, setPresetMetadata);
	setPresetMetadata(presetManager->metadata());
}

} // namespace PaintField
