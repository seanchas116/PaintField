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
#include <QRadioButton>
#include <QSignalMapper>
#include <QButtonGroup>

#include "paintfield/core/serializationutil.h"
#include "paintfield/core/widgets/widgetgroup.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/shapelayer.h"
#include "paintfield/core/rectlayer.h"
#include "paintfield/core/textlayer.h"
#include "paintfield/core/widgets/simplebutton.h"

#include "shapesidebar.h"

namespace PaintField {

struct ShapeSideBar::Data
{
	std::shared_ptr<const AbstractRectLayer> rectLayer;
	std::shared_ptr<const TextLayer> textLayer;
	
	QTextEdit *textEdit = 0;
	QToolButton *fontButton = 0;
	
	WidgetGroup *rectGroup = 0, *textGroup = 0;
	
	QSignalMapper *alignSignalMapper;
	QHash<int, QAbstractButton *> alignButtons;
};

ShapeSideBar::ShapeSideBar(LayerScene *scene, QWidget *parent) :
	AbstractLayerPropertyEditor(scene, parent),
	d(new Data)
{
	d->rectGroup = new WidgetGroup(this);
	d->textGroup = new WidgetGroup(this);
	
	d->alignSignalMapper = new QSignalMapper(this);
	connect(d->alignSignalMapper, SIGNAL(mapped(int)), this, SLOT(onTextAlignmentChanged(int)));
	
	auto layout = new QFormLayout();
	
	auto addRowWidget = [layout](const QString &text, QWidget *field, WidgetGroup *group)
	{
		auto label = new QLabel(text);
		layout->addRow(label, field);
		group->addWidget(label);
		group->addWidget(field);
	};
	
	auto addRowLayout = [layout](const QString &text, QLayout *field, WidgetGroup *group)
	{
		auto label = new QLabel(text);
		layout->addRow(label, field);
		group->addWidget(label);
		group->addLayout(field);
	};
	
	QFont boldFont;
	boldFont.setBold(true);
	
	{
		{
			auto label = new QLabel("RECTANGLE");
			label->setFont(boldFont);
			d->rectGroup->addWidget(label);
			layout->addRow(label);
		}
		
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
			
			addRowLayout(tr("Position"), hl, d->rectGroup);
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
			
			addRowLayout(tr("Size"), hl, d->rectGroup);
		}
	}
	
	{
		{
			auto label = new QLabel("TEXT");
			label->setFont(boldFont);
			d->textGroup->addWidget(label);
			layout->addRow(label);
		}
		
		{
			auto b = new QToolButton();
			connect(b, SIGNAL(pressed()), this, SLOT(onFontSelectRequested()));
			d->fontButton = b;
			
			addRowWidget(tr("Font"), b, d->textGroup);
		}
		
		{
			auto checkBox = new QCheckBox(tr("Italic"));
			connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(onFontItalicChanged(bool)));
			connect(this, SIGNAL(fontItalicChanged(bool)), checkBox, SLOT(setChecked(bool)));
			
			addRowWidget("", checkBox, d->textGroup);
		}
		
		{
			auto hlayout = new QHBoxLayout();
			auto hgroup = new QButtonGroup(this);
			auto vgroup = new QButtonGroup(this);
			
			auto addAlignButton = [hlayout, this](const QString &iconPath, int alignment, QButtonGroup *buttonGroup)
			{
				auto b = new SimpleButton(iconPath, QSize(16, 16));
				b->setCheckable(true);
				connect(b, SIGNAL(pressed()), d->alignSignalMapper, SLOT(map()));
				d->alignSignalMapper->setMapping(b, alignment);
				d->alignButtons[alignment] = b;
				hlayout->addWidget(b);
				buttonGroup->addButton(b);
			};
			
			addAlignButton(":/icons/16x16/alignLeft.svg", Qt::AlignLeft, hgroup);
			addAlignButton(":/icons/16x16/alignHCenter.svg", Qt::AlignHCenter, hgroup);
			addAlignButton(":/icons/16x16/alignRight.svg", Qt::AlignRight, hgroup);
			addAlignButton(":/icons/16x16/alignTop.svg", Qt::AlignTop, vgroup);
			addAlignButton(":/icons/16x16/alignVCenter.svg", Qt::AlignVCenter, vgroup);
			addAlignButton(":/icons/16x16/alignBottom.svg", Qt::AlignBottom, vgroup);
			
			addRowLayout(tr("Align"), hlayout, d->textGroup);
		}
		
		{
			auto t = new QTextEdit();
			connect(t, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
			d->textEdit = t;
			
			addRowWidget(tr("Text"), t, d->textGroup);
		}
	}
	
	setLayout(layout);
}

void ShapeSideBar::updateForCurrentChange(const LayerConstPtr &current)
{
	d->rectLayer = std::dynamic_pointer_cast<const AbstractRectLayer>(current);
	d->textLayer = std::dynamic_pointer_cast<const TextLayer>(current);
	
	updateForCurrentPropertyChange();
}

void ShapeSideBar::updateForCurrentPropertyChange()
{
	d->rectGroup->setVisible((bool)d->rectLayer);
	d->textGroup->setVisible((bool)d->textLayer);
	
	if (d->rectLayer)
	{
		emit xChanged(d->rectLayer->rect().x());
		emit yChanged(d->rectLayer->rect().y());
		emit widthChanged(d->rectLayer->rect().width());
		emit heightChanged(d->rectLayer->rect().height());
	}
	
	if (d->textLayer)
	{
		if (d->textEdit->toPlainText() != d->textLayer->text())
			d->textEdit->setText(d->textLayer->text());
		
		QFont font = d->textLayer->font();
		QString fontText = font.family() + " / " + font.styleName() + " / " + QString::number(font.pointSize()) + "pt";
		d->fontButton->setText(fontText);
		
		emit fontItalicChanged(font.italic());
		
		auto alignment = d->textLayer->alignment();
		
		for (Qt::Alignment alignElem : { Qt::AlignLeft, Qt::AlignHCenter, Qt::AlignRight, Qt::AlignTop, Qt::AlignVCenter, Qt::AlignBottom })
		{
			if (alignment & alignElem)
				d->alignButtons[alignElem]->setChecked(true);
		}
	}
}

void ShapeSideBar::onXChanged(int x)
{
	if (d->rectLayer)
	{
		QRectF rect = d->rectLayer->rect();
		rect.moveLeft(x);
		layerScene()->setLayerProperty(d->rectLayer, rect, RoleRect, tr("Change Rectangle x"));
	}
}

void ShapeSideBar::onYChanged(int y)
{
	if (d->rectLayer)
	{
		QRectF rect = d->rectLayer->rect();
		rect.moveTop(y);
		layerScene()->setLayerProperty(d->rectLayer, rect, RoleRect, tr("Change Rectangle y"));
	}
}

void ShapeSideBar::onWidthChanged(int w)
{
	if (d->rectLayer)
	{
		QRectF rect = d->rectLayer->rect();
		rect.setWidth(w);
		layerScene()->setLayerProperty(d->rectLayer, rect, RoleRect, tr("Change Rectangle Width"));
	}
}

void ShapeSideBar::onHeightChanged(int h)
{
	if (d->rectLayer)
	{
		QRectF rect = d->rectLayer->rect();
		rect.setHeight(h);
		layerScene()->setLayerProperty(d->rectLayer, rect, RoleRect, tr("Change Rectangle Height"));
	}
}

void ShapeSideBar::onTextChanged()
{
	if (d->textLayer)
	{
		auto text = d->textEdit->toPlainText();
		layerScene()->setLayerProperty(d->textLayer, text, RoleText, tr("Change Text"));
	}
}

void ShapeSideBar::onFontSelectRequested()
{
	if (d->textLayer)
	{
		bool ok;
		
		auto font = QFontDialog::getFont(&ok, d->textLayer->font(), 0, tr("Select Font"), QFontDialog::DontUseNativeDialog);
		if (ok)
			layerScene()->setLayerProperty(d->textLayer, font, RoleFont, tr("Change Font"));
	}
}

void ShapeSideBar::onFontItalicChanged(bool italic)
{
	if (d->textLayer)
	{
		auto font = d->textLayer->font();
		font.setItalic(italic);
		layerScene()->setLayerProperty(d->textLayer, font, RoleFont, tr("Change Font Italic"));
	}
}

void ShapeSideBar::onTextAlignmentChanged(int changedAlignmentInt)
{
	if (d->textLayer)
	{
		auto alignment = d->textLayer->alignment();
		auto changedAlignment = Qt::Alignment(changedAlignmentInt);
		
		switch (changedAlignment)
		{
			case Qt::AlignLeft:
			case Qt::AlignHCenter:
			case Qt::AlignRight:
			{
				alignment &= ~(Qt::AlignLeft | Qt::AlignHCenter | Qt::AlignRight);
				alignment |= changedAlignment;
				break;
			}
			case Qt::AlignTop:
			case Qt::AlignVCenter:
			case Qt::AlignBottom:
			{
				alignment &= ~(Qt::AlignTop | Qt::AlignVCenter | Qt::AlignBottom);
				alignment |= changedAlignment;
				break;
			}
			default:
				return;
		}
		
		layerScene()->setLayerProperty(d->textLayer, int(alignment), RoleTextAlignment, tr("Change Text Alignment"));
	}
}

} // namespace PaintField
