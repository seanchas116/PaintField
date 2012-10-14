#include <QtGui>
#include "core/application.h"
#include "widgets/colorbutton.h"
#include "widgets/colorslider.h"
#include "widgets/colorwheel.h"
#include "widgets/modulardoublespinbox.h"
#include "widgets/simplebutton.h"
#include "widgets/loosespinbox.h"
#include "widgets/widgetgroup.h"
#include "modules/palette/palettemodule.h"

#include "colorpanel.h"

namespace PaintField
{

using namespace Malachite;

inline void fsConnectMutual(QObject *object1, const char *signal, QObject *object2, const char *slot)
{
	QObject::connect(object1, signal, object2, slot);
	QObject::connect(object2, signal, object1, slot);
}

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
	
	hsvSliders << new ColorSlider(Color::Hue, 3600);
	hsvSliders << new ColorSlider(Color::Saturation, 1000);
	hsvSliders << new ColorSlider(Color::Value, 1000);
	
	rgb8Sliders << new ColorSlider(Color::Red, 1000);
	rgb8Sliders << new ColorSlider(Color::Green, 1000);
	rgb8Sliders << new ColorSlider(Color::Blue, 1000);
	
	for (int i = 0; i < 3; ++i)
	{
		fsConnectMutual(rgbSliders.at(i), SIGNAL(colorChanged(Color)), this, SLOT(setColor(Color)));
		fsConnectMutual(rgbSliders.at(i), SIGNAL(valueChanged(double)), rgbSpins.at(i), SLOT(setValue(double)));
		rgbSpins.at(i)->setValue(rgbSliders.at(i)->value());
	}
	
	for (int i = 0; i < 3; ++i)
	{
		fsConnectMutual(hsvSliders.at(i), SIGNAL(colorChanged(Color)), this, SLOT(setColor(Color)));
		fsConnectMutual(hsvSliders.at(i), SIGNAL(valueChanged(double)), hsvSpins.at(i), SLOT(setValue(double)));
		hsvSpins.at(i)->setValue(hsvSliders.at(i)->value());
	}
	
	for (int i = 0; i < 3; ++i)
	{
		fsConnectMutual(rgb8Sliders.at(i), SIGNAL(colorChanged(Color)), this, SLOT(setColor(Color)));
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
	
	_groupRgb = new WidgetGroup(this);
	_groupHsv = new WidgetGroup(this);
	_groupRgb8 = new WidgetGroup(this);
	
	_groupRgb->addWidgets(rgbLabels);
	_groupRgb->addWidgets(rgbSliders);
	_groupRgb->addWidgets(rgbSpins);
	
	_groupHsv->addWidgets(hsvLabels);
	_groupHsv->addWidgets(hsvSliders);
	_groupHsv->addWidgets(hsvSpins);
	
	_groupRgb8->addWidgets(rgb8Labels);
	_groupRgb8->addWidgets(rgb8Sliders);
	_groupRgb8->addWidgets(rgb8Spins);
	
	setLayout(layout);
	
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	
	comboBox->setCurrentIndex(0);
	onComboBoxActivated(0);
	setColor(Color::white());
}

void ColorSliderPanel::setColor(const Color &color)
{
	if (_color == color)
		return;
	
	_color = color;
	
	emit colorChanged(color);
}

void ColorSliderPanel::onComboBoxActivated(int index)
{
	_groupRgb->setVisible(false);
	_groupHsv->setVisible(false);
	_groupRgb8->setVisible(false);
	
	switch (index)
	{
		case 0:
			_groupRgb->setVisible(true);
			break;
		case 1:
			_groupHsv->setVisible(true);
			break;
		case 2:
			_groupRgb8->setVisible(true);
			break;
		default:
			break;
	}
}

WebColorPanel::WebColorPanel(QWidget *parent) :
	QWidget(parent)
{
	_lineEdit = new QLineEdit();
	
	_layout = new QFormLayout();
	_layout->setContentsMargins(QMargins(0, 0, 0, 0));
	_layout->addRow(tr("Web Color"), _lineEdit);
	
	setLayout(_layout);
	
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

ColorPanel::ColorPanel(QWidget *parent) :
	QWidget(parent)
{
	setWindowTitle(tr("Color"));
	
	// buttons
	
	_buttonLayout = new QHBoxLayout();
	
	_wheelButton = new SimpleButton(":/icons/24x24/colorWheel.svg");
	_sliderButton = new SimpleButton(":/icons/24x24/colorSlider.svg");
	_webButton = new SimpleButton(":/icons/24x24/webColor.svg");
	_dialogButton = new SimpleButton(":/icons/24x24/colorDialog.svg");
	
	_wheelButton->setCheckable(true);
	_sliderButton->setCheckable(true);
	_webButton->setCheckable(true);
	
	_buttonGroup = new QButtonGroup(this);
	_buttonGroup->addButton(_wheelButton);
	_buttonGroup->addButton(_sliderButton);
	_buttonGroup->addButton(_webButton);
	
	_wheelButton->setChecked(true);
	
	_buttonLayout->setMargin(0);
	_buttonLayout->addStretch(1);
	_buttonLayout->addWidget(_wheelButton);
	_buttonLayout->addWidget(_sliderButton);
	_buttonLayout->addWidget(_webButton);
	_buttonLayout->addWidget(_dialogButton);
	_buttonLayout->addStretch(1);
	
	// color buttons
	
	_colorButtonGroup = new QButtonGroup(this);
	
	_colorButtonLayout = new QHBoxLayout();
	_colorButtonLayout->setSpacing(0);
	
	_colorButtonLayout->addStretch(1);
	for (int i = 0; i < 7; ++i)
	{
		ColorButton *button = new ColorButton();
		button->setColor(Color::white());
		_colorButtons << button;
		_colorButtonLayout->addWidget(button);
		_colorButtonGroup->addButton(button);
		connect(button, SIGNAL(pressed()), this, SLOT(onColorButtonPressed()));
	}
	_colorButtonLayout->addStretch(1);
	
	// color wheel
	
	_colorWheel = new ColorWheel();
	
	// color sliders
	
	_sliderPanel = new ColorSliderPanel();
	
	// web color
	
	_webColorPanel = new WebColorPanel();
	
	// main layout
	
	_mainLayout = new QVBoxLayout();
	
	_mainLayout->addLayout(_buttonLayout);
	_mainLayout->addLayout(_colorButtonLayout);
	_mainLayout->addWidget(_colorWheel, 0, Qt::AlignCenter);
	_mainLayout->addWidget(_sliderPanel);
	_mainLayout->addWidget(_webColorPanel);
	_mainLayout->addStretch(1);
	
	setLayout(_mainLayout);
	
	connect(_colorWheel, SIGNAL(colorChanged(Color)), this, SLOT(setColor(Color)));
	connect(_sliderPanel, SIGNAL(colorChanged(Color)), this, SLOT(setColor(Color)));
	connect(_webColorPanel, SIGNAL(colorChanged(Color)), this, SLOT(setColor(Color)));
	
	connect(this, SIGNAL(currentColorChanged(Color)), _colorWheel, SLOT(setColor(Color)));
	connect(this, SIGNAL(currentColorChanged(Color)), _sliderPanel, SLOT(setColor(Color)));
	connect(this, SIGNAL(currentColorChanged(Color)), _webColorPanel, SLOT(setColor(Color)));
	
	for (int i = 0; i < 7; ++i)
	{
		_colorButtons.at(i)->setColor(pmanager->color(i));
	}
	setCurrentIndex(pmanager->currentIndex());
	
	_colorWheel->setColor(pmanager->currentColor());
	_sliderPanel->setColor(pmanager->currentColor());
	_webColorPanel->setColor(pmanager->currentColor());
	
	connect(_wheelButton, SIGNAL(toggled(bool)), _colorWheel, SLOT(setVisible(bool)));
	connect(_sliderButton, SIGNAL(toggled(bool)), _sliderPanel, SLOT(setVisible(bool)));
	connect(_webButton, SIGNAL(toggled(bool)), _webColorPanel, SLOT(setVisible(bool)));
	
	_wheelButton->setChecked(true);
	
	_colorWheel->setVisible(_wheelButton->isChecked());
	_sliderPanel->setVisible(_sliderButton->isChecked());
	_webColorPanel->setVisible(_webButton->isChecked());
}

void ColorPanel::setColor(int index, const Color &color)
{
	_colorButtons.at(index)->setColor(color);
	if (index == _currentIndex)
		emit currentColorChanged(color);
}

void ColorPanel::setCurrentIndex(int index)
{
	_currentIndex = 0;
	
	if (_colorButtons.at(index)->isChecked())
		return;
	
	_colorButtons.at(index)->setChecked(true);
}

void ColorPanel::onColorButtonPressed()
{
	int index = _colorButtons.indexOf(reinterpret_cast<ColorButton *>(sender()));
	if (index >= 0)
		PaletteModule::paletteManager()->setCurrentIndex(index);
}

}
