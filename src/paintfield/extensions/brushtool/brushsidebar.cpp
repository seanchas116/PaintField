#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>

#include "brushsidebar.h"

namespace PaintField {

BrushSideBar::BrushSideBar(QWidget *parent) :
    QWidget(parent)
{
	auto mainLayout = new QVBoxLayout;
	
	{
		auto label = new QLabel;
		mainLayout->addWidget(label);
		
		_presetLabel = label;
	}
	
	{
		auto prefLayout = new QGridLayout;
		
		prefLayout->addWidget(new QLabel("Size:"), 0, 0);
		
		{
			auto slider = new QSlider(Qt::Horizontal);
			slider->setMaximum(200);
			slider->setMinimum(1);
			
			slider->setValue(brushSize());
			connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setBrushSize(int)));
			connect(this, SIGNAL(brushSizeChanged(int)), slider, SLOT(setValue(int)));
			
			prefLayout->addWidget(slider, 0, 1);
		}
		
		{
			auto spinBox = new QSpinBox;
			spinBox->setMaximum(200);
			spinBox->setMinimum(1);
			
			spinBox->setValue(brushSize());
			connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(setBrushSize(int)));
			connect(this, SIGNAL(brushSizeChanged(int)), spinBox, SLOT(setValue(int)));
			
			prefLayout->addWidget(spinBox, 0, 2);
		}
		
		mainLayout->addLayout(prefLayout);
	}
	
	setLayout(mainLayout);
}

void BrushSideBar::setPresetMetadata(const BrushPresetMetadata &metadata)
{
	QString presetText;
	
	if (metadata.title().isEmpty())
		presetText = "Preset : <b>[Not Selected]</b>";
	else
		presetText = "Preset : <b>" + metadata.title() + "</b>";
	
	//_presetLabel->setText("<font size=\"+1\">" + presetText + "</font>");
	_presetLabel->setText(presetText);
}

void BrushSideBar::setBrushSize(int size)
{
	if (_brushSize != size)
	{
		_brushSize = size;
		emit brushSizeChanged(size);
	}
}

} // namespace PaintField
