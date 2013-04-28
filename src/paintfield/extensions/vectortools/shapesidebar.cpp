#include <QFormLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QFontDialog>
#include <QToolButton>
#include <QCheckBox>

#include "paintfield/core/serializationutil.h"
#include "paintfield/core/widgets/widgetgroup.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/shapelayer.h"
#include "paintfield/core/rectlayer.h"
#include "paintfield/core/textlayer.h"

#include "shapesidebar.h"

namespace PaintField {

struct ShapeSideBar::Data
{
	LayerScene *scene = 0;
	LayerRef current;
	
	QTextEdit *textEdit = 0;
	QToolButton *fontButton = 0;
	
	WidgetGroup *rectGroup = 0, *textGroup = 0;
	
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
	QWidget(parent),
	d(new Data)
{
	d->scene = scene;
	
	d->rectGroup = new WidgetGroup(this);
	d->textGroup = new WidgetGroup(this);
	
	auto layout = new QFormLayout();
	
	{
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
				d->rectGroup->addWidget(s);
			}
			
			{
				auto s = createSpinBox();
				connect(this, SIGNAL(yChanged(int)), s, SLOT(setValue(int)));
				connect(s, SIGNAL(valueChanged(int)), this, SLOT(onYChanged(int)));
				hl->addWidget(s);
				d->rectGroup->addWidget(s);
			}
			
			auto l = new QLabel(tr("Position"));
			d->rectGroup->addWidget(l);
			
			layout->addRow(l, hl);
		}
		
		{
			auto hl = new QHBoxLayout();
			
			{
				auto s = createSpinBox();
				connect(this, SIGNAL(widthChanged(int)), s, SLOT(setValue(int)));
				connect(s, SIGNAL(valueChanged(int)), this, SLOT(onWidthChanged(int)));
				hl->addWidget(s);
				d->rectGroup->addWidget(s);
			}
			
			{
				auto s = createSpinBox();
				connect(this, SIGNAL(heightChanged(int)), s, SLOT(setValue(int)));
				connect(s, SIGNAL(valueChanged(int)), this, SLOT(onHeightChanged(int)));
				hl->addWidget(s);
				d->rectGroup->addWidget(s);
			}
			
			auto l = new QLabel(tr("Size"));
			d->rectGroup->addWidget(l);
			
			layout->addRow(l, hl);
		}
	}
	
	{
		{
			auto b = new QToolButton();
			b->setText(tr("Change"));
			connect(b, SIGNAL(pressed()), this, SLOT(onFontSelectRequested()));
			d->textGroup->addWidget(b);
			
			auto l = new QLabel(tr("Font"));
			d->textGroup->addWidget(l);
			
			layout->addRow(l, b);
			d->fontButton = b;
		}
		
		{
			auto t = new QTextEdit();
			connect(t, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
			d->textEdit = t;
			d->textGroup->addWidget(t);
			
			auto l = new QLabel(tr("Font"));
			d->textGroup->addWidget(l);
			
			layout->addRow(l, t);
		}
		
		{
			auto checkBox = new QCheckBox(tr("Italic"));
			connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(onFontItalicChanged(bool)));
			connect(this, SIGNAL(fontItalicChanged(bool)), checkBox, SLOT(setChecked(bool)));
			
			auto l = new QLabel("");
			d->textGroup->addWidget(checkBox);
			d->textGroup->addWidget(l);
			
			layout->addRow(l, checkBox);
		}
	}
	
	setLayout(layout);
	
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

void ShapeSideBar::onTextChanged()
{
	auto layer = d->currentLayerWithType<TextLayer>();
	if (layer)
	{
		auto text = d->textEdit->toPlainText();
		d->scene->setLayerProperty(d->current, text, RoleText, tr("Change Text"));
	}
}

void ShapeSideBar::onFontSelectRequested()
{
	auto layer = d->currentLayerWithType<TextLayer>();
	if (layer)
	{
		bool ok;
		
		auto font = QFontDialog::getFont(&ok, layer->font(), 0, tr("Select Font"), QFontDialog::DontUseNativeDialog);
		//auto font = QFontDialog::getFont(&ok, layer->font(), 0, tr("Select Font"));
		if (ok)
		{
			d->scene->setLayerProperty(d->current, font, RoleFont, tr("Change Font"));
		}
	}
}

void ShapeSideBar::onFontItalicChanged(bool italic)
{
	auto layer = d->currentLayerWithType<TextLayer>();
	if (layer)
	{
		auto font = layer->font();
		font.setItalic(italic);
		d->scene->setLayerProperty(d->current, font, RoleFont, tr("Change Font"));
	}
}

void ShapeSideBar::updateEditors()
{
	auto rectLayer = d->currentLayerWithType<AbstractRectLayer>();
	auto textLayer = d->currentLayerWithType<TextLayer>();
	
	d->rectGroup->setVisible(rectLayer);
	d->textGroup->setVisible(textLayer);
	
	if (rectLayer)
	{
		emit xChanged(rectLayer->rect().x());
		emit yChanged(rectLayer->rect().y());
		emit widthChanged(rectLayer->rect().width());
		emit heightChanged(rectLayer->rect().height());
	}
	
	if (textLayer)
	{
		if (d->textEdit->toPlainText() != textLayer->text())
			d->textEdit->setText(textLayer->text());
		
		QFont font = textLayer->font();
		QString fontText = font.family() + " / " + font.styleName() + " / " + QString::number(font.pointSize()) + "pt";
		d->fontButton->setText(fontText);
		
		emit fontItalicChanged(font.italic());
	}
}

} // namespace PaintField
