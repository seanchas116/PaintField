#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QFormLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <stdexcept>

#include "paintfield/core/util.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/widgets/colorbutton.h"
#include "paintfield/core/widgets/colorslider.h"
#include "paintfield/core/widgets/colorwheel.h"
#include "paintfield/core/widgets/modulardoublespinbox.h"
#include "paintfield/core/widgets/simplebutton.h"
#include "paintfield/core/widgets/loosespinbox.h"
#include "paintfield/core/widgets/widgetgroup.h"
#include "paintfield/core/widgets/doubleslider.h"
#include "paintfield/core/signalconverter.h"
#include "paintfield/core/workspace.h"

#include "colorsidebar.h"

namespace PaintField
{

using namespace Malachite;
using namespace std;

ColorSliderPanel::ColorSliderPanel(QWidget *parent) :
	QWidget(parent)
{
	auto layout = new QVBoxLayout;
	
	auto wgroupRgb = new WidgetGroup;
	auto wgroupHsv = new WidgetGroup;
	auto wgroupRgb8 = new WidgetGroup;
	
	{
		auto buttonLayout = new QHBoxLayout;
		
		buttonLayout->addStretch(1);
		
		{
			auto b = new QPushButton(tr("RGB"));
			b->setCheckable(true);
			connect(b, SIGNAL(toggled(bool)), wgroupRgb, SLOT(setVisible(bool)));
			buttonLayout->addWidget(b);
			
			b->setChecked(true);
		}
		
		{
			auto b = new QPushButton(tr("HSV"));
			b->setCheckable(true);
			connect(b, SIGNAL(toggled(bool)), wgroupHsv, SLOT(setVisible(bool)));
			buttonLayout->addWidget(b);
		}
		
		{
			auto b = new QPushButton(tr("RGB 0..255"));
			b->setCheckable(true);
			connect(b, SIGNAL(toggled(bool)), wgroupRgb8, SLOT(setVisible(bool)));
			buttonLayout->addWidget(b);
		}
		
		buttonLayout->addStretch(1);
		
		layout->addLayout(buttonLayout);
	}
	
	{
		auto gridLayout = new QGridLayout;
		
		auto addToLayout = [gridLayout](const QString &label, QWidget *slider, QWidget *spin, WidgetGroup *wgroup)
		{
			auto labelWidget = new QLabel(label);
			
			int row = gridLayout->rowCount();
			gridLayout->addWidget(labelWidget, row, 0);
			gridLayout->addWidget(slider, row, 1);
			gridLayout->addWidget(spin, row, 2);
			
			wgroup->addWidget(labelWidget);
			wgroup->addWidget(slider);
			wgroup->addWidget(spin);
		};
		
		auto addLine = [this, addToLayout](const QString &label, ColorSlider *slider, LooseSpinBox *spin, WidgetGroup *wgroup)
		{
			addToLayout(label, slider, spin, wgroup);
			Util::connectMutual(slider, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(setColor(Malachite::Color)));
			Util::connectMutual(slider, SIGNAL(valueChanged(double)), spin, SLOT(setValue(double)));
		};
		
		auto createLooseSpinBox = []()
		{
			auto *spin = new LooseSpinBox();
			spin->setDecimals(3);
			spin->setSingleStep(0.01);
			spin->setMinimum(0);
			spin->setMaximum(1);
			spin->setMinimumWidth(60);
			return spin;
		};
		
		addLine("R", new ColorSlider(Color::Red, 1000), createLooseSpinBox(), wgroupRgb);
		addLine("G", new ColorSlider(Color::Green, 1000), createLooseSpinBox(), wgroupRgb);
		addLine("B", new ColorSlider(Color::Blue, 1000), createLooseSpinBox(), wgroupRgb);
		
		{
			auto addLineHue = [this, addToLayout](const QString &label, ColorSlider *slider, LooseSpinBox *spin, WidgetGroup *wgroup)
			{
				addToLayout(label, slider, spin, wgroup);
				
				auto normalizedToDegrees = [](double x) { return x * 360.0; };
				auto degreesToNormalized = [](double x) { return x * (1.0 / 360.0); };
				
				auto signalConverter = SignalConverter::fromDoubleFunc(normalizedToDegrees, degreesToNormalized, this);
				signalConverter->connectChannelADouble(slider, SIGNAL(valueChanged(double)), SLOT(setValue(double)));
				signalConverter->connectChannelBDouble(spin, SIGNAL(valueChanged(double)), SLOT(setValue(double)));
				Util::connectMutual(slider, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(setColor(Malachite::Color)));
			};
			
			LooseSpinBox *spin = new LooseSpinBox();
			spin->setDecimals(1);
			spin->setSingleStep(1);
			spin->setMinimum(0);
			spin->setMaximum(360);
			spin->setWrapping(true);
			spin->setMinimumWidth(60);
			
			addLineHue("H", new ColorSlider(Color::Hue, 1000), spin, wgroupHsv);
		}
		
		addLine("S", new ColorSlider(Color::Saturation, 1000), createLooseSpinBox(), wgroupHsv);
		addLine("V", new ColorSlider(Color::Value, 1000), createLooseSpinBox(), wgroupHsv);
		
		{
			auto createIntSpinBox = []
			{
				auto spin = new QSpinBox();
				spin->setSingleStep(1);
				spin->setMinimum(0);
				spin->setMaximum(255);
				spin->setMinimumWidth(60);
				return spin;
			};
			
			auto addLineInt = [this, addToLayout](const QString &label, ColorSlider *slider, QSpinBox *spin, WidgetGroup *wgroup)
			{
				addToLayout(label, slider, spin, wgroup);
				
				Util::connectMutual(slider, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(setColor(Malachite::Color)));
				connect(slider, SIGNAL(value8BitChanged(int)), spin, SLOT(setValue(int)));
				connect(spin, SIGNAL(valueChanged(int)), slider, SLOT(setValue8Bit(int)));
				spin->setValue(slider->value8Bit());
			};
			
			addLineInt("R", new ColorSlider(Color::Red, 1000), createIntSpinBox(), wgroupRgb8);
			addLineInt("G", new ColorSlider(Color::Green, 1000), createIntSpinBox(), wgroupRgb8);
			addLineInt("B", new ColorSlider(Color::Blue, 1000), createIntSpinBox(), wgroupRgb8);
		}
		
		layout->addLayout(gridLayout);
	}
	
	setLayout(layout);
}

void ColorSliderPanel::setColor(const Color &color)
{
	if (_color == color) return;
	
	_color = color;
	emit colorChanged(color);
}

void ColorSliderPanel::onComboBoxActivated(int index)
{
	emit rgbSelectedChanged(false);
	emit rgb8SelectedChanged(false);
	emit hsvSelectedChanged(false);
	
	switch (index)
	{
		case 0:
			emit rgbSelectedChanged(true);
			break;
		case 1:
			emit hsvSelectedChanged(true);
			break;
		case 2:
			emit rgb8SelectedChanged(true);
			break;
		default:
			break;
	}
}

WebColorPanel::WebColorPanel(QWidget *parent) :
	QWidget(parent)
{
	_lineEdit = new QLineEdit();
	
	QFormLayout *layout = new QFormLayout();
	layout->setContentsMargins(QMargins(0, 0, 0, 0));
	layout->addRow(tr("Web Color"), _lineEdit);
	
	setLayout(layout);
	
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	
	connect(_lineEdit, SIGNAL(editingFinished()), this, SLOT(onLineEditEditingFinished()));
}

void WebColorPanel::setColor(const Color &color)
{
	if (_color == color)
		return;
	
	_color = color;
	_lineEdit->setText(color.toWebColor());
	
	emit colorChanged(color);
}

void WebColorPanel::onLineEditEditingFinished()
{
	try
	{
		Color newColor = Color::fromWebColor(_lineEdit->text());
		setColor(newColor);
	}
	catch (const runtime_error &)
	{
		_lineEdit->setText(color().toWebColor());
	}
}

ColorSideBar::ColorSideBar(QWidget *parent) :
	QWidget(parent)
{
	auto mainLayout = new QVBoxLayout();
	
	auto wheelButton = new SimpleButton(":/icons/24x24/colorWheel.svg", QSize(24,24));
	auto sliderButton = new SimpleButton(":/icons/24x24/colorSlider.svg", QSize(24,24));
	auto webButton = new SimpleButton(":/icons/24x24/webColor.svg", QSize(24,24));
	auto dialogButton = new SimpleButton(":/icons/24x24/colorDialog.svg", QSize(24,24));
	
	wheelButton->setCheckable(true);
	sliderButton->setCheckable(true);
	webButton->setCheckable(true);
	
	// buttons
	{
		auto layout = new QHBoxLayout();
		
		layout->setMargin(0);
		layout->addStretch(1);
		layout->addWidget(wheelButton);
		layout->addWidget(sliderButton);
		layout->addWidget(webButton);
		layout->addWidget(dialogButton);
		layout->addStretch(1);
		
		mainLayout->addLayout(layout);
	}
	
	// color buttons
	{
		auto layout = new QHBoxLayout();
		
		layout->setSpacing(0);
		layout->addStretch(1);
		
		for (int i = 0; i < 7; ++i)
		{
			auto button = new ColorButton();
			_colorButtons << button;
			layout->addWidget(button);
			connect(button, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(onColorButtonChanged(Malachite::Color)));
		}
		
		layout->addStretch(1);
		
		mainLayout->addLayout(layout);
	}
	
	// widgets
	
	auto colorWheel = new ColorWheel();
	auto sliderPanel = new ColorSliderPanel();
	auto webColorPanel = new WebColorPanel();
	
	{
		mainLayout->addWidget(colorWheel, 0, Qt::AlignCenter);
		mainLayout->addWidget(sliderPanel);
		mainLayout->addWidget(webColorPanel);
		
		connect(colorWheel, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(setCurrentColor(Malachite::Color)));
		connect(sliderPanel, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(setCurrentColor(Malachite::Color)));
		connect(webColorPanel, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(setCurrentColor(Malachite::Color)));
		
		connect(this, SIGNAL(currentColorChanged(Malachite::Color)), colorWheel, SLOT(setColor(Malachite::Color)));
		connect(this, SIGNAL(currentColorChanged(Malachite::Color)), sliderPanel, SLOT(setColor(Malachite::Color)));
		connect(this, SIGNAL(currentColorChanged(Malachite::Color)), webColorPanel, SLOT(setColor(Malachite::Color)));
		
		connect(wheelButton, SIGNAL(toggled(bool)), colorWheel, SLOT(setVisible(bool)));
		connect(sliderButton, SIGNAL(toggled(bool)), sliderPanel, SLOT(setVisible(bool)));
		connect(webButton, SIGNAL(toggled(bool)), webColorPanel, SLOT(setVisible(bool)));
	}
	
	mainLayout->addStretch(1);
	
	// opacity
	{
		auto layout = new QHBoxLayout;
		
		layout->addWidget(new QLabel(tr("Opacity:")));
		
		{
			auto slider = new DoubleSlider(Qt::Horizontal);
			slider->setMinimum(0);
			slider->setMaximum(1000);
			slider->setDoubleMinimum(0);
			slider->setDoubleMaximum(100);
			
			auto spinBox = new LooseSpinBox;
			spinBox->setDecimals(1);
			spinBox->setMinimum(0);
			spinBox->setMaximum(100);
			spinBox->setSingleStep(1);
			
			connect(slider, SIGNAL(doubleValueChanged(double)), spinBox, SLOT(setValue(double)));
			connect(spinBox, SIGNAL(valueChanged(double)), slider, SLOT(setDoubleValue(double)));
			
			auto toPercent = [](double x) { return x * 100; };
			auto fromPercent = [](double x) { return x / 100; };
			
			auto converter = SignalConverter::fromDoubleFunc(toPercent, fromPercent,this);
			converter->connectChannelADouble(this, SIGNAL(currentOpacityChanged(double)), SLOT(setCurrentOpacity(double)));
			converter->connectChannelBDouble(slider, SIGNAL(doubleValueChanged(double)), SLOT(setDoubleValue(double)));
			
			layout->addWidget(slider);
			layout->addWidget(spinBox);
			layout->addWidget(new QLabel(tr("%")));
		}
		
		mainLayout->addLayout(layout);
	}
	
	setLayout(mainLayout);
	
	colorWheel->setVisible(false);
	sliderPanel->setVisible(false);
	webColorPanel->setVisible(false);
	
	wheelButton->setChecked(true);
}

void ColorSideBar::setColorButtonColor(int index, const Color &color)
{
	_colorButtons.at(index)->setColor(color);
}

void ColorSideBar::setCurrentColor(const Color &color)
{
	if (_color != color)
	{
		_color = color;
		emit currentColorChanged(color);
		emit currentOpacityChanged(color.alpha());
	}
}

void ColorSideBar::setCurrentOpacity(double opacity)
{
	auto color = _color;
	color.setAlpha(opacity);
	setCurrentColor(color);
}

void ColorSideBar::onColorButtonChanged(const Color &color)
{
	auto button = qobject_cast<ColorButton *>(sender());
	if (button && _colorButtons.contains(button))
	{
		emit colorButtonColorChanged(_colorButtons.indexOf(button), color);
	}
}

}
