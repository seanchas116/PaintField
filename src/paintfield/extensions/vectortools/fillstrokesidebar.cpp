#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>
#include <cfloat>
#include <QButtonGroup>
#include <QFormLayout>
#include <QGroupBox>

#include "paintfield/core/shapelayer.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/widgets/doubleslider.h"
#include "paintfield/core/widgets/loosespinbox.h"
#include "paintfield/core/widgets/colorbutton.h"
#include "paintfield/core/workspace.h"
#include "paintfield/core/colorbuttongroup.h"

#include "fillstrokesidebar.h"

using namespace Malachite;

namespace PaintField {

struct FillStrokeSideBar::Data
{
	LayerScene *scene = 0;
	const ShapeLayer *current = 0;
	
	QButtonGroup *strokePosGroup = 0;
};

FillStrokeSideBar::FillStrokeSideBar(Workspace *workspace, LayerScene *scene, QWidget *parent) :
	QWidget(parent),
	d(new Data)
{
	d->scene = scene;
	
	{
		auto vLayout = new QVBoxLayout();
		vLayout->setContentsMargins(6,6,6,6);
		
		{
			auto l = new QHBoxLayout();
			l->setAlignment(Qt::AlignVCenter);
			
			auto check = new QCheckBox(tr("Fill"));
			connect(check, SIGNAL(toggled(bool)), this, SLOT(onFillEnabledToggled(bool)));
			connect(this, SIGNAL(fillEnabledChanged(bool)), check, SLOT(setChecked(bool)));
			l->addWidget(check);
			
			auto color = new ColorButton();
			connect(color, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(onFillColorSet(Malachite::Color)));
			connect(this, SIGNAL(fillColorChanged(Malachite::Color)), color, SLOT(setColor(Malachite::Color)));
			l->addWidget(color);
			
			if (workspace)
				workspace->colorButtonGroup()->add(color);
			
			vLayout->addLayout(l);
		}
		
		{
			auto l = new QHBoxLayout();
			
			auto check = new QCheckBox(tr("Stroke"));
			connect(check, SIGNAL(toggled(bool)), this, SLOT(onStrokeEnabledToggled(bool)));
			connect(this, SIGNAL(strokeEnabledChanged(bool)), check, SLOT(setChecked(bool)));
			l->addWidget(check);
			
			auto color = new ColorButton();
			connect(color, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(onStrokeColorSet(Malachite::Color)));
			connect(this, SIGNAL(strokeColorChanged(Malachite::Color)), color, SLOT(setColor(Malachite::Color)));
			l->addWidget(color);
			
			if (workspace)
				workspace->colorButtonGroup()->add(color);
			
			vLayout->addLayout(l);
		}
		
		{
			auto strokeWidget = new QWidget();
			
			auto strokeLayout = new QVBoxLayout();
			strokeLayout->setContentsMargins(12, 0, 0, 0);
			
			{
				auto l = new QHBoxLayout();
				
				l->addWidget(new QLabel(tr("Width:")));
				
				auto spin = new LooseSpinBox();
				spin->setDecimals(1);
				spin->setSingleStep(1.0);
				
				connect(spin, SIGNAL(valueChanged(double)), this, SLOT(onStrokeWidthSet(double)));
				connect(this, SIGNAL(strokeWidthChanged(double)), spin, SLOT(setValue(double)));
				
				l->addWidget(spin);
				
				l->addStretch(1);
				
				strokeLayout->addLayout(l);
			}
			
			{
				auto l = new QHBoxLayout();
				
				d->strokePosGroup = new QButtonGroup(this);
				connect(d->strokePosGroup, SIGNAL(buttonPressed(int)), this, SLOT(onStrokePosButtonPressed(int)));
				
				{
					auto radio = new QRadioButton(tr("Inside"));
					d->strokePosGroup->addButton(radio, StrokePositionInside);
					l->addWidget(radio);
				}
				
				{
					auto radio = new QRadioButton(tr("Center"));
					d->strokePosGroup->addButton(radio, StrokePositionCenter);
					l->addWidget(radio);
				}
				
				{
					auto radio = new QRadioButton(tr("Outside"));
					d->strokePosGroup->addButton(radio, StrokePositionOutside);
					l->addWidget(radio);
				}
				
				strokeLayout->addLayout(l);
			}
			
			strokeWidget->setLayout(strokeLayout);
			vLayout->addWidget(strokeWidget);
		}
		
		vLayout->addStretch(1);
		
		setLayout(vLayout);
	}
	
	if (scene)
	{
		connect(scene, SIGNAL(currentChanged(LayerRef,LayerRef)), this, SLOT(setCurrentLayer(LayerRef)));
		connect(scene, SIGNAL(currentLayerPropertyChanged()), this, SLOT(updateEditor()));
		setCurrentLayer(scene->current());
	}
	else
	{
		setCurrentLayer(0);
	}
}

FillStrokeSideBar::~FillStrokeSideBar()
{
	delete d;
}

void FillStrokeSideBar::setCurrentLayer(const LayerRef &current)
{
	if (current)
		d->current = dynamic_cast<const ShapeLayer *>(current.pointer());
	else
		d->current = 0;
	
	updateEditor();
}

void FillStrokeSideBar::updateEditor()
{
	setEnabled(d->current);
	
	if (d->current)
	{
		d->strokePosGroup->button(d->current->strokePosition())->setChecked(true);
		
		fillEnabledChanged(d->current->isFillEnabled());
		strokeEnabledChanged(d->current->isStrokeEnabled());
		strokeWidthChanged(d->current->strokeWidth());
		fillColorChanged(d->current->fillBrush().color());
		strokeColorChanged(d->current->strokeBrush().color());
	}
}

void FillStrokeSideBar::onStrokePosButtonPressed(int id)
{
	if (!d->current)
		return;
	
	d->scene->setLayerProperty(d->current, id, RoleStrokePosition, tr("Change Stroke Position"));
}

void FillStrokeSideBar::onFillEnabledToggled(bool checked)
{
	if (!d->current)
		return;
	
	QString text = checked ? tr("Enable Fill") : tr("Disable Fill");
	d->scene->setLayerProperty(d->current, checked, RoleFillEnabled, text);
}

void FillStrokeSideBar::onStrokeEnabledToggled(bool checked)
{
	if (!d->current)
		return;
	
	QString text = checked ? tr("Enable Stroke") : tr("Disable Stroke");
	d->scene->setLayerProperty(d->current, checked, RoleStrokeEnabled, text);
}

void FillStrokeSideBar::onStrokeWidthSet(double width)
{
	if (!d->current)
		return;
	
	PAINTFIELD_DEBUG << width;
	
	d->scene->setLayerProperty(d->current, width, RoleStrokeWidth, tr("Change Stroke Width"));
}

void FillStrokeSideBar::onFillColorSet(const Color &color)
{
	if (!d->current)
		return;
	
	if (d->current->fillBrush().color() != color)
	{
		PAINTFIELD_DEBUG << color.alpha() << color.red() << color.green() << color.blue();
		d->scene->setLayerProperty(d->current, QVariant::fromValue(Brush(color)), RoleFillBrush, tr("Change Fill Color"));
	}
}

void FillStrokeSideBar::onStrokeColorSet(const Color &color)
{
	if (!d->current)
		return;
	
	if (d->current->strokeBrush().color() != color)
	{
		PAINTFIELD_DEBUG << color.alpha() << color.red() << color.green() << color.blue();
		d->scene->setLayerProperty(d->current, QVariant::fromValue(Brush(color)), RoleStrokeBrush, tr("Change Stroke Color"));
	}
}

} // namespace PaintField
