#include <QFormLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QFontDialog>
#include <QToolButton>

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
	
	{
		auto w = new QWidget();
		auto layout = new QFormLayout();
		
		{
			auto b = new QToolButton();
			b->setText(tr("Change"));
			connect(b, SIGNAL(pressed()), this, SLOT(onFontChangeRequested()));
			layout->addRow(tr("Font"), b);
			d->fontButton = b;
		}
		
		{
			auto t = new QTextEdit();
			connect(t, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
			d->textEdit = t;
			layout->addRow(tr("Text"), t);
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

void ShapeSideBar::onTextChanged()
{
	auto layer = d->currentLayerWithType<TextLayer>();
	if (layer)
	{
		auto text = d->textEdit->toPlainText();
		d->scene->setLayerProperty(d->current, text, RoleText, tr("Change Text"));
	}
}

void ShapeSideBar::onFontChangeRequested()
{
	auto layer = d->currentLayerWithType<TextLayer>();
	if (layer)
	{
		bool ok;
		auto font = QFontDialog::getFont(&ok, layer->font(), 0, tr("Select Font"), QFontDialog::DontUseNativeDialog);
		if (ok)
		{
			PAINTFIELD_DEBUG << font.pointSize();
			d->scene->setLayerProperty(d->current, font, RoleFont, tr("Change Font"));
		}
	}
}

void ShapeSideBar::updateEditors()
{
	auto rectLayer = d->currentLayerWithType<RectLayer>();
	auto textLayer = d->currentLayerWithType<TextLayer>();
	
	if (rectLayer)
		setCurrentIndex(StackIndexRect);
	else if (textLayer)
		setCurrentIndex(StackIndexText);
	else
		setCurrentIndex(StackIndexEmpty);
	
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
	}
}

} // namespace PaintField
