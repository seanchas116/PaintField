#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMenu>
#include <QComboBox>
#include <QLabel>

#include "paintfield/core/document.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/widgets/doubleslider.h"
#include "paintfield/core/widgets/loosespinbox.h"

#include "layerpropertyeditor.h"

namespace PaintField
{

struct LayerPropertyEditor::Data
{
	LayerScene *scene = 0;
	LayerConstPtr current;
	
	DoubleSlider *opacitySlider = 0;
	LooseSpinBox *opacitySpinBox = 0;
	QComboBox *blendModeComboBox = 0;
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
			auto combo = new QComboBox();
			combo->addItem(tr("Unimplemented"));
			d->blendModeComboBox = combo;
			
			formLayout->addRow(tr("Blend"), combo);
		}
		
		setLayout(formLayout);
	}
	
	if (scene)
	{
		connect(scene, SIGNAL(currentLayerChanged()), this, SLOT(updateEditor()));
		connect(scene, SIGNAL(currentChanged(LayerConstPtr,LayerConstPtr)), this, SLOT(setCurrentLayer(LayerConstPtr)));
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

void LayerPropertyEditor::setCurrentLayer(const LayerConstPtr &current)
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
		d->opacitySpinBox->setValue(d->current->property(RoleOpacity).toDouble() * 100.0);
	}
}

void LayerPropertyEditor::setOpacityPercent(double value)
{
	d->scene->setLayerProperty(d->current, value / 100.0, RoleOpacity, tr("Set Layer Opacity"));
}

}
