#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>
#include <cfloat>
#include <QButtonGroup>
#include <QFormLayout>
#include <QGroupBox>
#include <QSignalMapper>

#include "paintfield/core/widgets/simplebutton.h"
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
	const ShapeLayer *current = 0;
	
	QHash<int, QAbstractButton *> strokePosButtons, joinStyleButtons, capStyleButtons;
};

FillStrokeSideBar::FillStrokeSideBar(Workspace *workspace, LayerScene *scene, QWidget *parent) :
	AbstractLayerPropertyEditor(scene, parent),
	d(new Data)
{
	auto formLayout = new QFormLayout();
	
	QFont boldFont;
	boldFont.setBold(true);
	
	// fill heading with checkbox
	{
		auto check = new QCheckBox(tr("FILL"));
		check->setFont(boldFont);
		connect(check, SIGNAL(toggled(bool)), this, SLOT(onFillEnabledToggled(bool)));
		connect(this, SIGNAL(fillEnabledChanged(bool)), check, SLOT(setChecked(bool)));
		formLayout->addRow(check);
	}
	
	// fill color
	{
		auto color = new ColorButton();
		color->setToolTip(tr("Click to edit color"));
		connect(color, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(onFillColorSet(Malachite::Color)));
		connect(this, SIGNAL(fillColorChanged(Malachite::Color)), color, SLOT(setColor(Malachite::Color)));
		formLayout->addRow(tr("Color"), color);
		
		if (workspace)
			workspace->colorButtonGroup()->add(color);
	}
	
	{
		auto check = new QCheckBox(tr("STROKE"));
		check->setFont(boldFont);
		connect(check, SIGNAL(toggled(bool)), this, SLOT(onStrokeEnabledToggled(bool)));
		connect(this, SIGNAL(strokeEnabledChanged(bool)), check, SLOT(setChecked(bool)));
		formLayout->addRow(check);
	}
	
	{
		auto color = new ColorButton();
		color->setToolTip(tr("Click to edit color"));
		connect(color, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(onStrokeColorSet(Malachite::Color)));
		connect(this, SIGNAL(strokeColorChanged(Malachite::Color)), color, SLOT(setColor(Malachite::Color)));
		formLayout->addRow(tr("Color"), color);
		
		if (workspace)
			workspace->colorButtonGroup()->add(color);
	}
	
	{
		auto spin = new LooseSpinBox();
		connect(this, SIGNAL(strokeWidthChanged(double)), spin, SLOT(setValue(double)));
		connect(spin, SIGNAL(valueChanged(double)), this, SLOT(onStrokeWidthSet(double)));
		formLayout->addRow(tr("Width"), spin);
	}
	
	auto addButtonFunction = [this](QHBoxLayout *layout, QSignalMapper *mapper, QHash<int, QAbstractButton *> *buttonHash)
	{
		auto group = new QButtonGroup(this);
		
		return [=](const QString &iconPath, int id)
		{
			auto b = new SimpleButton(iconPath, QSize(16, 16));
			b->setCheckable(true);
			mapper->setMapping(b, id);
			connect(b, SIGNAL(pressed()), mapper, SLOT(map()));
			(*buttonHash)[id] = b;
			layout->addWidget(b);
			group->addButton(b);
		};
	};
	
	{
		auto hlayout = new QHBoxLayout();
		auto mapper = new QSignalMapper(this);
		connect(mapper, SIGNAL(mapped(int)), this, SLOT(onStrokePosChanged(int)));
		
		auto addButton = addButtonFunction(hlayout, mapper, &d->strokePosButtons);
		addButton(":/icons/16x16/strokeInside.svg", StrokePositionInside);
		addButton(":/icons/16x16/strokeCenter.svg", StrokePositionCenter);
		addButton(":/icons/16x16/strokeOutside.svg", StrokePositionOutside);
		
		formLayout->addRow(tr("Position"), hlayout);
	}
	
	{
		auto hlayout = new QHBoxLayout();
		auto mapper = new QSignalMapper(this);
		connect(mapper, SIGNAL(mapped(int)), this, SLOT(onStrokeCapChanged(int)));
		
		auto addButton = addButtonFunction(hlayout, mapper, &d->capStyleButtons);
		addButton(":/icons/16x16/capFlat.svg", Qt::FlatCap);
		addButton(":/icons/16x16/capSquare.svg", Qt::SquareCap);
		addButton(":/icons/16x16/capRound.svg", Qt::RoundCap);
		
		formLayout->addRow(tr("Cap"), hlayout);
	}
	
	{
		auto hlayout = new QHBoxLayout();
		auto mapper = new QSignalMapper(this);
		connect(mapper, SIGNAL(mapped(int)), this, SLOT(onStrokeJoinChanged(int)));
		
		auto addButton = addButtonFunction(hlayout, mapper, &d->joinStyleButtons);
		addButton(":/icons/16x16/joinBevel.svg", Qt::BevelJoin);
		addButton(":/icons/16x16/joinMiter.svg", Qt::MiterJoin);
		addButton(":/icons/16x16/joinRound.svg", Qt::RoundJoin);
		
		formLayout->addRow(tr("Join"), hlayout);
	}
	
	setLayout(formLayout);
	
	updateForCurrentPropertyChange();
}

FillStrokeSideBar::~FillStrokeSideBar()
{
	delete d;
}

void FillStrokeSideBar::updateForCurrentChange(const LayerRef &current)
{
	if (current)
		d->current = dynamic_cast<const ShapeLayer *>(current.pointer());
	else
		d->current = 0;
	
	updateForCurrentPropertyChange();
}

void FillStrokeSideBar::updateForCurrentPropertyChange()
{
	setEnabled(d->current);
	
	if (d->current)
	{
		{
			auto b = d->strokePosButtons.value(d->current->strokePosition(), 0);
			if (b)
				b->setChecked(true);
		}
		
		{
			auto b = d->capStyleButtons.value(d->current->capStyle(), 0);
			if (b)
				b->setChecked(true);
		}
		
		{
			auto b = d->joinStyleButtons.value(d->current->joinStyle(), 0);
			if (b)
				b->setChecked(true);
		}
		
		fillEnabledChanged(d->current->isFillEnabled());
		strokeEnabledChanged(d->current->isStrokeEnabled());
		strokeWidthChanged(d->current->strokeWidth());
		fillColorChanged(d->current->fillBrush().color());
		strokeColorChanged(d->current->strokeBrush().color());
	}
}

void FillStrokeSideBar::onStrokePosChanged(int strokePos)
{
	if (d->current)
	{
		layerScene()->setLayerProperty(d->current, strokePos, RoleStrokePosition, tr("Change Stroke Position"));
	}
}

void FillStrokeSideBar::onStrokeJoinChanged(int join)
{
	if (d->current)
	{
		layerScene()->setLayerProperty(d->current, join, RoleJoinStyle, tr("Change Stroke Join Style"));
	}
}

void FillStrokeSideBar::onStrokeCapChanged(int cap)
{
	if (d->current)
	{
		layerScene()->setLayerProperty(d->current, cap, RoleCapStyle, tr("Change Stroke Cap Style"));
	}
}

void FillStrokeSideBar::onFillEnabledToggled(bool checked)
{
	if (!d->current)
		return;
	
	QString text = checked ? tr("Enable Fill") : tr("Disable Fill");
	layerScene()->setLayerProperty(d->current, checked, RoleFillEnabled, text);
}

void FillStrokeSideBar::onStrokeEnabledToggled(bool checked)
{
	if (!d->current)
		return;
	
	QString text = checked ? tr("Enable Stroke") : tr("Disable Stroke");
	layerScene()->setLayerProperty(d->current, checked, RoleStrokeEnabled, text);
}

void FillStrokeSideBar::onStrokeWidthSet(double width)
{
	if (!d->current)
		return;
	
	layerScene()->setLayerProperty(d->current, width, RoleStrokeWidth, tr("Change Stroke Width"));
}

void FillStrokeSideBar::onFillColorSet(const Color &color)
{
	if (!d->current)
		return;
	
	if (d->current->fillBrush().color() != color)
		layerScene()->setLayerProperty(d->current, QVariant::fromValue(Brush(color)), RoleFillBrush, tr("Change Fill Color"));
}

void FillStrokeSideBar::onStrokeColorSet(const Color &color)
{
	if (!d->current)
		return;
	
	if (d->current->strokeBrush().color() != color)
		layerScene()->setLayerProperty(d->current, QVariant::fromValue(Brush(color)), RoleStrokeBrush, tr("Change Stroke Color"));
}

} // namespace PaintField
