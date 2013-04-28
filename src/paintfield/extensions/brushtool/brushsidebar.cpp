#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QFormLayout>

#include "brushsidebar.h"

namespace PaintField {

BrushSideBar::BrushSideBar(QWidget *parent) :
    QWidget(parent)
{
	auto formLayout = new QFormLayout();
	
	{
		auto label = new QLabel();
		formLayout->addRow(tr("Preset"), label);
		_presetLabel = label;
	}
	
	{
		auto hlayout = new QHBoxLayout();
		
		{
			auto slider = new QSlider(Qt::Horizontal);
			slider->setMaximum(200);
			slider->setMinimum(1);
			
			slider->setValue(brushSize());
			connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setBrushSize(int)));
			connect(this, SIGNAL(brushSizeChanged(int)), slider, SLOT(setValue(int)));
			
			hlayout->addWidget(slider, 0);
		}
		
		{
			auto spinBox = new QSpinBox;
			spinBox->setMaximum(200);
			spinBox->setMinimum(1);
			
			spinBox->setValue(brushSize());
			connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(setBrushSize(int)));
			connect(this, SIGNAL(brushSizeChanged(int)), spinBox, SLOT(setValue(int)));
			
			hlayout->addWidget(spinBox, 0);
		}
		
		formLayout->addRow(tr("Size"), hlayout);
	}
	
	setLayout(formLayout);
}

void BrushSideBar::setPresetMetadata(const BrushPresetMetadata &metadata)
{
	QString presetText;
	
	if (metadata.title().isEmpty())
		presetText = "<b>[Not Selected]</b>";
	else
		presetText = "<b>" + metadata.title() + "</b>";
	
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
