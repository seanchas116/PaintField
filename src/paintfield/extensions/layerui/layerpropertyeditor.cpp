#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMenu>
#include <QComboBox>
#include <QLabel>

#include "paintfield/core/grouplayer.h"
#include "paintfield/core/document.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/widgets/doubleslider.h"
#include "paintfield/core/widgets/loosespinbox.h"
#include "paintfield/core/blendmodetexts.h"

#include "layerpropertyeditor.h"

namespace PaintField
{

using namespace Malachite;

struct LayerPropertyEditor::Data
{
	LayerScene *scene = 0;
	LayerConstRef current;
	bool isCurrentGroup = false;
	
	DoubleSlider *opacitySlider = 0;
	LooseSpinBox *opacitySpinBox = 0;
	QComboBox *blendModeComboBox = 0;
	
	QList<int> comboBoxBlendModes;
};

LayerPropertyEditor::LayerPropertyEditor(LayerScene *scene, QWidget *parent) :
	QWidget(parent),
	d(new Data)
{
	
	d->scene = scene;
	
	{
		auto formLayout = new QFormLayout();
		formLayout->setSpacing(5);
		formLayout->setContentsMargins(5,5,5,5);
		
		// opacity
		{
			auto layout = new QHBoxLayout();
			
			{
				auto slider = new DoubleSlider(Qt::Horizontal);
				slider->setMinimum(0);
				slider->setMaximum(1000);
				slider->setDoubleMinimum(0.0);
				slider->setDoubleMaximum(100.0);
				d->opacitySlider = slider;
				
				auto spin = new LooseSpinBox();
				spin->setDecimals(1);
				spin->setMinimum(0);
				spin->setMaximum(100);
				spin->setSingleStep(1.0);
				d->opacitySpinBox = spin;
				
				connect(slider, SIGNAL(doubleValueChanged(double)), spin, SLOT(setValue(double)));
				connect(spin, SIGNAL(valueChanged(double)), slider, SLOT(setDoubleValue(double)));
				connect(slider, SIGNAL(doubleValueChanged(double)), this, SLOT(setOpacityPercent(double)));
				
				layout->addWidget(slider);
				layout->addWidget(spin);
				layout->addWidget(new QLabel("%"));
			}
			
			formLayout->addRow(tr("Opacity"), layout);
		}
		
		{
			d->comboBoxBlendModes = 
			{
				BlendMode::Normal,
				-1,
				BlendMode::Lighten,
				BlendMode::Screen,
				BlendMode::ColorDodge,
				BlendMode::Plus,
				-1,
				BlendMode::Darken,
				BlendMode::Multiply,
				BlendMode::ColorBurn,
				-1,
				BlendMode::Overlay,
				BlendMode::SoftLight,
				BlendMode::HardLight,
				-1,
				BlendMode::Difference,
				BlendMode::Exclusion,
				-1,
				BlendMode::Hue,
				BlendMode::Saturation,
				BlendMode::Color,
				BlendMode::Luminosity,
				-1,
				BlendMode::PassThrough
			};
			
			auto combo = new QComboBox();
			connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onBlendModeComboBoxChanged(int)));
			d->blendModeComboBox = combo;
			
			updateComboBoxItems();
			
			formLayout->addRow(tr("Blend"), combo);
		}
		
		setLayout(formLayout);
	}
	
	if (scene)
	{
		connect(scene, SIGNAL(currentLayerChanged()), this, SLOT(updateEditor()));
		connect(scene, SIGNAL(currentChanged(LayerConstRef,LayerConstRef)), this, SLOT(setCurrentLayer(LayerConstRef)));
		setCurrentLayer(scene->current());
	}
	else
	{
		updateEditor();
	}
}

LayerPropertyEditor::~LayerPropertyEditor()
{
	delete d;
}

void LayerPropertyEditor::setCurrentLayer(const LayerConstRef &current)
{
	PAINTFIELD_DEBUG << current;
	d->current = current;
	updateEditor();
}

void LayerPropertyEditor::updateEditor()
{
	setEnabled((bool)d->current);
	
	if (d->current)
	{
		bool isCurrentGroup = d->current->isType<GroupLayer>();
		
		if (d->isCurrentGroup != isCurrentGroup)
		{
			d->isCurrentGroup = isCurrentGroup;
			updateComboBoxItems();
		}
		
		d->opacitySpinBox->setValue(d->current->property(RoleOpacity).toDouble() * 100.0);
		
		d->blendModeComboBox->setCurrentIndex(d->comboBoxBlendModes.indexOf(d->current->blendMode().toInt()));
	}
}

void LayerPropertyEditor::setOpacityPercent(double value)
{
	d->scene->setLayerProperty(d->current, value / 100.0, RoleOpacity, tr("Set Layer Opacity"));
}

void LayerPropertyEditor::onBlendModeComboBoxChanged(int index)
{
	if (d->current)
		d->scene->setLayerProperty(d->current, d->comboBoxBlendModes.at(index), RoleBlendMode, tr("Set Blend Mode"));
}

void LayerPropertyEditor::updateComboBoxItems()
{
	disconnect(d->blendModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onBlendModeComboBoxChanged(int)));
	d->blendModeComboBox->clear();
	
	for (int blendMode : d->comboBoxBlendModes)
	{
		if (blendMode == -1)
			d->blendModeComboBox->insertSeparator(d->blendModeComboBox->count());
		else
			d->blendModeComboBox->addItem(BlendModeTexts::text(blendMode));
		
		if (blendMode == BlendMode::Luminosity && !d->isCurrentGroup)
			break;
	}
	
	connect(d->blendModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onBlendModeComboBoxChanged(int)));
}

}
