#include <QtGui>
#include "paintfield-core/util.h"
#include "paintfield-core/appcontroller.h"
#include "paintfield-core/widgets/colorbutton.h"
#include "paintfield-core/widgets/colorslider.h"
#include "paintfield-core/widgets/colorwheel.h"
#include "paintfield-core/widgets/modulardoublespinbox.h"
#include "paintfield-core/widgets/simplebutton.h"
#include "paintfield-core/widgets/loosespinbox.h"
#include "paintfield-core/widgets/widgetgroup.h"
#include "paintfield-core/widgets/doubleslider.h"
#include "paintfield-core/signalconverter.h"

#include "colorsidebar.h"

namespace PaintField
{

using namespace Malachite;

ColorSliderPanel::ColorSliderPanel(QWidget *parent) :
	QWidget(parent)
{
	QList<LooseSpinBox *> rgbSpins, hsvSpins;
	QList<QSpinBox *> rgb8Spins;
	
	for (int i = 0; i < 3; ++i)
	{
		LooseSpinBox *spin = new LooseSpinBox();
		spin->setDecimals(3);
		spin->setSingleStep(0.01);
		spin->setMinimum(0);
		spin->setMaximum(1);
		spin->setMinimumWidth(60);
		rgbSpins << spin;
	}
	
	{
		LooseSpinBox *spin = new LooseSpinBox();
		spin->setDecimals(1);
		spin->setSingleStep(1);
		spin->setMinimum(0);
		spin->setMaximum(360);
		spin->setWrapping(true);
		spin->setMinimumWidth(60);
		hsvSpins << spin;
	}
	
	for (int i = 0; i < 2; ++i)
	{
		LooseSpinBox *spin = new LooseSpinBox();
		spin->setDecimals(3);
		spin->setSingleStep(0.01);
		spin->setMinimum(0);
		spin->setMaximum(1);
		spin->setMinimumWidth(60);
		hsvSpins << spin;
	}
	
	for (int i = 0; i < 3; ++i)
	{
		QSpinBox *spin = new QSpinBox();
		spin->setSingleStep(1);
		spin->setMinimum(0);
		spin->setMaximum(255);
		spin->setMinimumWidth(60);
		rgb8Spins << spin;
	}
	
	QList<ColorSlider *> rgbSliders, hsvSliders, rgb8Sliders;
	
	rgbSliders << new ColorSlider(Color::Red, 1000);
	rgbSliders << new ColorSlider(Color::Green, 1000);
	rgbSliders << new ColorSlider(Color::Blue, 1000);
	
	hsvSliders << new ColorSlider(Color::Hue, 1000);
	hsvSliders << new ColorSlider(Color::Saturation, 1000);
	hsvSliders << new ColorSlider(Color::Value, 1000);
	
	rgb8Sliders << new ColorSlider(Color::Red, 1000);
	rgb8Sliders << new ColorSlider(Color::Green, 1000);
	rgb8Sliders << new ColorSlider(Color::Blue, 1000);
	
	for (int i = 0; i < 3; ++i)
	{
 		Util::connectMutual(rgbSliders.at(i), SIGNAL(colorChanged(Malachite::Color)), this, SLOT(setColor(Malachite::Color)));
		Util::connectMutual(rgbSliders.at(i), SIGNAL(valueChanged(double)), rgbSpins.at(i), SLOT(setValue(double)));
		rgbSpins.at(i)->setValue(rgbSliders.at(i)->value());
	}
	
	{
		auto normalizedToDegrees = [](double x) { return x * 360.0; };
		auto degreesToNormalized = [](double x) { return x * (1.0 / 360.0); };
		
		auto signalConverter = SignalConverter::fromDoubleFunc(normalizedToDegrees, degreesToNormalized, this);
		signalConverter->connectChannelADouble(hsvSliders.at(0), SIGNAL(valueChanged(double)), SLOT(setValue(double)));
		signalConverter->connectChannelBDouble(hsvSpins.at(0), SIGNAL(valueChanged(double)), SLOT(setValue(double)));
		Util::connectMutual(hsvSliders.at(0), SIGNAL(colorChanged(Malachite::Color)), this, SLOT(setColor(Malachite::Color)));
	}
	
	for (int i = 1; i < 3; ++i)
	{
		Util::connectMutual(hsvSliders.at(i), SIGNAL(colorChanged(Malachite::Color)), this, SLOT(setColor(Malachite::Color)));
		Util::connectMutual(hsvSliders.at(i), SIGNAL(valueChanged(double)), hsvSpins.at(i), SLOT(setValue(double)));
		hsvSpins.at(i)->setValue(hsvSliders.at(i)->value());
	}
	
	for (int i = 0; i < 3; ++i)
	{
		Util::connectMutual(rgb8Sliders.at(i), SIGNAL(colorChanged(Malachite::Color)), this, SLOT(setColor(Malachite::Color)));
		connect(rgb8Sliders.at(i), SIGNAL(value8BitChanged(int)), rgb8Spins.at(i), SLOT(setValue(int)));
		connect(rgb8Spins.at(i), SIGNAL(valueChanged(int)), rgb8Sliders.at(i), SLOT(setValue8Bit(int)));
		rgb8Spins.at(i)->setValue(rgb8Sliders.at(i)->value8Bit());
	}
	
	QList<QLabel *> rgbLabels, hsvLabels, rgb8Labels;
	
	rgbLabels << new QLabel("R");
	rgbLabels << new QLabel("G");
	rgbLabels << new QLabel("B");
	hsvLabels << new QLabel("H");
	hsvLabels << new QLabel("S");
	hsvLabels << new QLabel("V");
	rgb8Labels << new QLabel("R");
	rgb8Labels << new QLabel("G");
	rgb8Labels << new QLabel("B");
	
	QComboBox *comboBox = new QComboBox();
	comboBox->addItem(tr("RGB"));
	comboBox->addItem(tr("HSV"));
	comboBox->addItem(tr("RGB (0..255)"));
	
	connect(comboBox, SIGNAL(activated(int)), this, SLOT(onComboBoxActivated(int)));
	
	QHBoxLayout *comboBoxLayout = new QHBoxLayout();
	comboBoxLayout->addWidget(comboBox);
	comboBoxLayout->addStretch(1);
	comboBoxLayout->setContentsMargins(0, 0, 0, 0);
	
	QGridLayout *layout = new QGridLayout();
	layout->addLayout(comboBoxLayout, 0, 1, 1, 2);
	
	for (int i = 0; i < 3; ++i)
	{
		layout->addWidget(rgbLabels.at(i), i+1, 0);
		layout->addWidget(rgbSliders.at(i), i+1, 1);
		layout->addWidget(rgbSpins.at(i), i+1, 2);
	}
	
	for (int i = 0; i < 3; ++i)
	{
		layout->addWidget(hsvLabels.at(i), i+4, 0);
		layout->addWidget(hsvSliders.at(i), i+4, 1);
		layout->addWidget(hsvSpins.at(i), i+4, 2);
	}
	
	for (int i = 0; i < 3; ++i)
	{
		layout->addWidget(rgb8Labels.at(i), i+7, 0);
		layout->addWidget(rgb8Sliders.at(i), i+7, 1);
		layout->addWidget(rgb8Spins.at(i), i+7, 2);
	}
	
	layout->setContentsMargins(0, 0, 0, 0);
	
	WidgetGroup *groupRgb = new WidgetGroup(this);
	WidgetGroup *groupHsv = new WidgetGroup(this);
	WidgetGroup *groupRgb8 = new WidgetGroup(this);
	
	groupRgb->addWidgets(rgbLabels);
	groupRgb->addWidgets(rgbSliders);
	groupRgb->addWidgets(rgbSpins);
	
	groupHsv->addWidgets(hsvLabels);
	groupHsv->addWidgets(hsvSliders);
	groupHsv->addWidgets(hsvSpins);
	
	groupRgb8->addWidgets(rgb8Labels);
	groupRgb8->addWidgets(rgb8Sliders);
	groupRgb8->addWidgets(rgb8Spins);
	
	connect(this, SIGNAL(rgbSelectedChanged(bool)), groupRgb, SLOT(setVisible(bool)));
	connect(this, SIGNAL(rgb8SelectedChanged(bool)), groupRgb8, SLOT(setVisible(bool)));
	connect(this, SIGNAL(hsvSelectedChanged(bool)), groupHsv, SLOT(setVisible(bool)));
	
	setLayout(layout);
	
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	
	comboBox->setCurrentIndex(0);
	onComboBoxActivated(0);
	setColor(Color::white());
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
	bool ok;
	Color newColor = Color::fromWebColor(_lineEdit->text(), &ok);
	
	if (ok)
	{
		setColor(newColor);
	}
	else
	{
		_lineEdit->setText(color().toWebColor());
	}
}

ColorSidebar::ColorSidebar(QWidget *parent) :
	QWidget(parent)
{
	setWindowTitle(tr("Color"));
	
	// buttons
	
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	
	SimpleButton *wheelButton = new SimpleButton(":/icons/24x24/colorWheel.svg", QSize(24,24)),
			*sliderButton = new SimpleButton(":/icons/24x24/colorSlider.svg", QSize(24,24)),
			*webButton = new SimpleButton(":/icons/24x24/webColor.svg", QSize(24,24)),
			*dialogButton = new SimpleButton(":/icons/24x24/colorDialog.svg", QSize(24,24));
	
	wheelButton->setCheckable(true);
	sliderButton->setCheckable(true);
	webButton->setCheckable(true);
	
	QButtonGroup *buttonGroup = new QButtonGroup(this);
	buttonGroup->addButton(wheelButton);
	buttonGroup->addButton(sliderButton);
	buttonGroup->addButton(webButton);
	
	buttonLayout->setMargin(0);
	buttonLayout->addStretch(1);
	buttonLayout->addWidget(wheelButton);
	buttonLayout->addWidget(sliderButton);
	buttonLayout->addWidget(webButton);
	buttonLayout->addWidget(dialogButton);
	buttonLayout->addStretch(1);
	
	// color buttons
	
	QButtonGroup *colorButtonGroup = new QButtonGroup(this);
	
	QHBoxLayout *colorButtonLayout = new QHBoxLayout();
	colorButtonLayout->setSpacing(0);
	colorButtonLayout->addStretch(1);
	
	for (int i = 0; i < 7; ++i)
	{
		ColorButton *button = new ColorButton();
		button->setColor(Color::white());
		_colorButtons << button;
		colorButtonLayout->addWidget(button);
		colorButtonGroup->addButton(button);
		connect(button, SIGNAL(pressed()), this, SLOT(onColorButtonPressed()));
	}
	colorButtonLayout->addStretch(1);
	
	// color wheel
	
	 ColorWheel *colorWheel = new ColorWheel();
	
	// color sliders
	
	ColorSliderPanel *sliderPanel = new ColorSliderPanel();
	
	// web color
	
	WebColorPanel *webColorPanel = new WebColorPanel();
	
	// main layout
	
	QVBoxLayout *mainLayout = new QVBoxLayout();
	
	mainLayout->addLayout(buttonLayout);
	mainLayout->addLayout(colorButtonLayout);
	mainLayout->addWidget(colorWheel, 0, Qt::AlignCenter);
	mainLayout->addWidget(sliderPanel);
	mainLayout->addWidget(webColorPanel);
	mainLayout->addStretch(1);
	
	setLayout(mainLayout);
	
	connect(colorWheel, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(setCurrentColor(Malachite::Color)));
	connect(sliderPanel, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(setCurrentColor(Malachite::Color)));
	connect(webColorPanel, SIGNAL(colorChanged(Malachite::Color)), this, SLOT(setCurrentColor(Malachite::Color)));
	
	connect(this, SIGNAL(currentColorChanged(Malachite::Color)), colorWheel, SLOT(setColor(Malachite::Color)));
	connect(this, SIGNAL(currentColorChanged(Malachite::Color)), sliderPanel, SLOT(setColor(Malachite::Color)));
	connect(this, SIGNAL(currentColorChanged(Malachite::Color)), webColorPanel, SLOT(setColor(Malachite::Color)));
	
	connect(wheelButton, SIGNAL(toggled(bool)), colorWheel, SLOT(setVisible(bool)));
	connect(sliderButton, SIGNAL(toggled(bool)), sliderPanel, SLOT(setVisible(bool)));
	connect(webButton, SIGNAL(toggled(bool)), webColorPanel, SLOT(setVisible(bool)));
	
	wheelButton->setChecked(true);
	
	colorWheel->setVisible(wheelButton->isChecked());
	sliderPanel->setVisible(sliderButton->isChecked());
	webColorPanel->setVisible(webButton->isChecked());
	
	setCurrentIndex(0);
	
	{
		// opacity
		
		auto layout = new QHBoxLayout;
		
		layout->addWidget(new QLabel("Alpha:"));
		
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
		}
		
		mainLayout->addLayout(layout);
	}
}

void ColorSidebar::setColor(int index, const Color &color)
{
	if (_colorButtons.at(index)->color() != color)
	{
		_colorButtons.at(index)->setColor(color);
		emit colorChanged(index, color);
		if (index == _currentIndex)
		{
			emit currentColorChanged(color);
			emit currentOpacityChanged(color.alpha());
		}
	}
}

void ColorSidebar::setCurrentIndex(int index)
{
	if (_currentIndex != index)
	{
		_currentIndex = index;
		
		emit currentColorChanged(_colorButtons.at(index)->color());
		emit currentIndexChanged(index);
		
		if (!_colorButtons.at(index)->isChecked())
			_colorButtons.at(index)->setChecked(true);
	}
}

void ColorSidebar::onColorButtonPressed()
{
	int index = _colorButtons.indexOf(reinterpret_cast<ColorButton *>(sender()));
	
	if (index >= 0)
		setCurrentIndex(index);
}

void ColorSidebar::setCurrentOpacity(double opacity)
{
	qDebug() << opacity;
	
	Color currentColor = _colorButtons.at(_currentIndex)->color();
	currentColor.setAlpha(opacity);
	setCurrentColor(currentColor);
}

}
