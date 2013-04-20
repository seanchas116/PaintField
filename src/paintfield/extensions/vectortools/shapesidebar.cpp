#include <QFormLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include "paintfield/core/widgets/widgetgroup.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/shapelayer.h"
#include "paintfield/core/rectlayer.h"

#include "shapesidebar.h"

namespace PaintField {

struct ShapeSideBar::Data
{
	LayerScene *scene = 0;
	LayerRef current;
	
	template <typename T>
	const T *currentLayerWithType()
	{
		if (current)
			return dynamic_cast<const T *>(current.pointer());
		else
			return 0;
	}
};

ShapeSideBar::ShapeSideBar(LayerScene *scene, QWidget *parent) :
	QStackedWidget(parent),
	d(new Data)
{
	d->scene = scene;
	
	addWidget(new QWidget());
	
	{
		auto w = new QWidget();
		
		auto layout = new QFormLayout();
		
		auto createSpinBox = []()
		{
			auto s = new QSpinBox();
			s->setRange(INT_MIN, INT_MAX);
			s->setMaximumWidth(60);
			s->setKeyboardTracking(false);
			return s;
		};
		
		{
			auto hl = new QHBoxLayout();
			
			{
				auto s = createSpinBox();
				connect(this, SIGNAL(xChanged(int)), s, SLOT(setValue(int)));
				connect(s, SIGNAL(valueChanged(int)), this, SLOT(onXChanged(int)));
				hl->addWidget(s);
			}
			
			{
				auto s = createSpinBox();
				connect(this, SIGNAL(yChanged(int)), s, SLOT(setValue(int)));
				connect(s, SIGNAL(valueChanged(int)), this, SLOT(onYChanged(int)));
				hl->addWidget(s);
			}
			
			layout->addRow(tr("Position"), hl);
		}
		
		{
			auto hl = new QHBoxLayout();
			
			{
				auto s = createSpinBox();
				connect(this, SIGNAL(widthChanged(int)), s, SLOT(setValue(int)));
				connect(s, SIGNAL(valueChanged(int)), this, SLOT(onWidthChanged(int)));
				hl->addWidget(s);
			}
			
			{
				auto s = createSpinBox();
				connect(this, SIGNAL(heightChanged(int)), s, SLOT(setValue(int)));
				connect(s, SIGNAL(valueChanged(int)), this, SLOT(onHeightChanged(int)));
				hl->addWidget(s);
			}
			
			layout->addRow(tr("Size"), hl);
		}
		
		w->setLayout(layout);
		addWidget(w);
	}
	
	connect(scene, SIGNAL(currentChanged(LayerRef,LayerRef)), this, SLOT(onCurrentChanged(LayerRef)));
	connect(scene, SIGNAL(currentLayerPropertyChanged()), this, SLOT(updateEditors()));
	onCurrentChanged(scene->current());
}

void ShapeSideBar::onCurrentChanged(const LayerRef &layer)
{
	d->current = layer;
	updateEditors();
}

void ShapeSideBar::onXChanged(int x)
{
	auto layer = d->currentLayerWithType<RectLayer>();
	if (layer)
	{
		QRectF rect = layer->rect();
		rect.moveLeft(x);
		d->scene->setLayerProperty(d->current, rect, RoleRect, tr("Change Rectangle X"));
	}
}

void ShapeSideBar::onYChanged(int y)
{
	auto layer = d->currentLayerWithType<RectLayer>();
	if (layer)
	{
		QRectF rect = layer->rect();
		rect.moveTop(y);
		d->scene->setLayerProperty(d->current, rect, RoleRect, tr("Change Rectangle Y"));
	}
}

void ShapeSideBar::onWidthChanged(int w)
{
	auto layer = d->currentLayerWithType<RectLayer>();
	if (layer)
	{
		QRectF rect = layer->rect();
		rect.setWidth(w);
		d->scene->setLayerProperty(d->current, rect, RoleRect, tr("Change Rectangle Width"));
	}
}

void ShapeSideBar::onHeightChanged(int h)
{
	auto layer = d->currentLayerWithType<RectLayer>();
	if (layer)
	{
		QRectF rect = layer->rect();
		rect.setHeight(h);
		d->scene->setLayerProperty(d->current, rect, RoleRect, tr("Change Rectangle Height"));
	}
}

void ShapeSideBar::onRectShapeTypeChanged(int type)
{
	
}

void ShapeSideBar::onTextChanged(const QString &text)
{
	
}

void ShapeSideBar::updateEditors()
{
	auto rectLayer = d->currentLayerWithType<RectLayer>();
	
	if (rectLayer)
		setCurrentIndex(StackIndexRect);
	else
		setCurrentIndex(StackIndexEmpty);
	
	if (rectLayer)
	{
		emit xChanged(rectLayer->rect().x());
		emit yChanged(rectLayer->rect().y());
		emit widthChanged(rectLayer->rect().width());
		emit heightChanged(rectLayer->rect().height());
	}
}

} // namespace PaintField
