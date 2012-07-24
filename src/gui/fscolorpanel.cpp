#include <QtGui>
#include "fscore.h"
#include "fscolorbutton.h"
#include "fscolorslider.h"
#include "fscolorwheel.h"
#include "fsdoubleedit.h"
#include "fssimplebutton.h"

#include "fscolorpanel.h"

FSColorSliderPanel::FSColorSliderPanel(QWidget *parent) :
	QWidget(parent)
{
	for (int i = 0; i < 6; ++i)
	{
		FSDoubleEdit *edit = new FSDoubleEdit();
		edit->setUnit(0.001);
		edit->setMaximumWidth(60);
		_lineEdits << edit;
	}
	
	_lineEdits.at(3)->setUnit(0.1);
	
	_sliders << new FSColorSlider(MLColor::Red);
	_sliders << new FSColorSlider(MLColor::Green);
	_sliders << new FSColorSlider(MLColor::Blue);
	_sliders << new FSColorSlider(MLColor::Hue);
	_sliders << new FSColorSlider(MLColor::Saturation);
	_sliders << new FSColorSlider(MLColor::Value);
	
	foreach (FSColorSlider *slider, _sliders)
	{
		slider->setStepCount(1000);
	}
	_sliders.at(3)->setStepCount(3600);
	
	for (int i = 0; i < 6; ++i)
	{
		connect(_sliders.at(i), SIGNAL(colorChanged(MLColor)), this, SLOT(setColor(MLColor)));
		connect(this, SIGNAL(colorChanged(MLColor)), _sliders.at(i), SLOT(setColor(MLColor)));
		connect(_sliders.at(i), SIGNAL(valueChanged(double)), _lineEdits.at(i), SLOT(setValue(double)));
		connect(_lineEdits.at(i), SIGNAL(valueChanged(double)), _sliders.at(i), SLOT(setValue(double)));
	}
	
	_labels << new QLabel("R");
	_labels << new QLabel("G");
	_labels << new QLabel("B");
	_labels << new QLabel("H");
	_labels << new QLabel("S");
	_labels << new QLabel("V");
	
	_comboBox = new QComboBox();
	_comboBox->addItem(tr("RGB"));
	_comboBox->addItem(tr("HSV"));
	
	connect(_comboBox, SIGNAL(activated(int)), this, SLOT(notifyComboBoxActivated(int)));
	
	_comboBox->setCurrentIndex(0);
	notifyComboBoxActivated(0);
	
	_comboBoxLayout = new QHBoxLayout();
	_comboBoxLayout->addWidget(_comboBox);
	_comboBoxLayout->addStretch(1);
	_comboBoxLayout->setContentsMargins(0, 0, 0, 0);
	
	_layout = new QGridLayout();
	
	_layout->addLayout(_comboBoxLayout, 0, 1, 1, 2);
	
	for (int i = 0; i < 6; ++i)
	{
		_layout->addWidget(_labels.at(i), i + 1, 0);
		_layout->addWidget(_sliders.at(i), i + 1, 1);
		_layout->addWidget(_lineEdits.at(i), i + 1, 2);
	}
	
	_layout->setContentsMargins(0, 0, 0, 0);
	
	setLayout(_layout);
	
	setColor(MLColor::fromRgb(1, 1, 1));
	
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
}

void FSColorSliderPanel::setColor(const MLColor &color)
{
	if (_color == color)
		return;
	
	_color = color;
	
	emit colorChanged(color);
}

void FSColorSliderPanel::notifyComboBoxActivated(int index)
{
	switch (index)
	{
	case 0:
		
		for (int i = 0; i < 3; ++i)
		{
			_labels.at(i)->setVisible(true);
			_sliders.at(i)->setVisible(true);
			_lineEdits.at(i)->setVisible(true);
		}
		for (int i = 3; i < 6; ++i)
		{
			_labels.at(i)->setVisible(false);
			_sliders.at(i)->setVisible(false);
			_lineEdits.at(i)->setVisible(false);
		}
		
		break;
	case 1:
		
		for (int i = 0; i < 3; ++i)
		{
			_labels.at(i)->setVisible(false);
			_sliders.at(i)->setVisible(false);
			_lineEdits.at(i)->setVisible(false);
		}
		for (int i = 3; i < 6; ++i)
		{
			_labels.at(i)->setVisible(true);
			_sliders.at(i)->setVisible(true);
			_lineEdits.at(i)->setVisible(true);
		}
		
		break;
	default:
		break;
	}
}

FSWebColorPanel::FSWebColorPanel(QWidget *parent) :
	QWidget(parent)
{
	_lineEdit = new QLineEdit();
	
	_layout = new QFormLayout();
	_layout->setContentsMargins(QMargins(0, 0, 0, 0));
	_layout->addRow(tr("Web Color"), _lineEdit);
	
	setLayout(_layout);
	
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	
	connect(_lineEdit, SIGNAL(editingFinished()), this, SLOT(notifyLineEditEditingFinished()));
}

void FSWebColorPanel::setColor(const MLColor &color)
{
	if (_color == color)
		return;
	
	_color = color;
	_lineEdit->setText(color.toWebColor());
	
	emit colorChanged(color);
}

void FSWebColorPanel::notifyLineEditEditingFinished()
{
	bool ok;
	MLColor newColor = MLColor::fromWebColor(_lineEdit->text(), &ok);
	if (ok)
		setColor(newColor);
}

FSColorPanel::FSColorPanel(QWidget *parent) :
	FSPanel(parent)
{
	setWindowTitle(tr("Color"));
	
	
	// buttons
	
	_buttonLayout = new QHBoxLayout();
	
	_wheelButton = new FSSimpleButton(":/icons/24x24/colorWheel.svg");
	_sliderButton = new FSSimpleButton(":/icons/24x24/colorSlider.svg");
	_webButton = new FSSimpleButton(":/icons/24x24/webColor.svg");
	_dialogButton = new FSSimpleButton(":/icons/24x24/colorDialog.svg");
	//_wheelButton = new FSSimpleButton(":/icons/24x24/colorWheel.svg", ":/icons/24x24/colorWheelOn.svg", ":/icons/24x24/colorWheelActive.svg", ":/icons/24x24/colorWheelActiveOn.svg");
	//_sliderButton = new FSSimpleButton(":/icons/24x24/colorSlider.svg", ":/icons/24x24/colorSliderOn.svg", ":/icons/24x24/colorSliderActive.svg", ":/icons/24x24/colorSliderActiveOn.svg");
	//_webButton = new FSSimpleButton(":/icons/24x24/webColor.svg", ":/icons/24x24/webColorOn.svg", ":/icons/24x24/webColorActive.svg", ":/icons/24x24/webColorActiveOn.svg");
	
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
		FSColorButton *button = new FSColorButton();
		button->setColor(MLColor::fromRgb(1, 1, 1));
		_colorButtons << button;
		_colorButtonLayout->addWidget(button);
		_colorButtonGroup->addButton(button);
		connect(button, SIGNAL(pressed()), this, SLOT(notifyColorButtonPressed()));
	}
	_colorButtonLayout->addStretch(1);
	
	// color wheel
	
	_colorWheel = new FSColorWheel();
	
	// color sliders
	
	_sliderPanel = new FSColorSliderPanel();
	
	// web color
	
	_webColorPanel = new FSWebColorPanel();
	
	// main layout
	
	_mainLayout = new QVBoxLayout();
	
	_mainLayout->addLayout(_buttonLayout);
	_mainLayout->addLayout(_colorButtonLayout);
	_mainLayout->addWidget(_colorWheel, 0, Qt::AlignCenter);
	_mainLayout->addWidget(_sliderPanel);
	_mainLayout->addWidget(_webColorPanel);
	_mainLayout->addStretch(1);
	
	setLayout(_mainLayout);
	
	connect(_colorWheel, SIGNAL(colorChanged(MLColor)), fsCore()->paletteManager(), SLOT(setColor(MLColor)));
	connect(_sliderPanel, SIGNAL(colorChanged(MLColor)), fsCore()->paletteManager(), SLOT(setColor(MLColor)));
	connect(_webColorPanel, SIGNAL(colorChanged(MLColor)), fsCore()->paletteManager(), SLOT(setColor(MLColor)));
	
	connect(fsCore()->paletteManager(), SIGNAL(currentColorChanged(MLColor)), _colorWheel, SLOT(setColor(MLColor)));
	connect(fsCore()->paletteManager(), SIGNAL(currentColorChanged(MLColor)), _sliderPanel, SLOT(setColor(MLColor)));
	connect(fsCore()->paletteManager(), SIGNAL(currentColorChanged(MLColor)), _webColorPanel, SLOT(setColor(MLColor)));
	
	connect(fsCore()->paletteManager(), SIGNAL(colorChanged(int,MLColor)), this, SLOT(setColor(int,MLColor)));
	connect(fsCore()->paletteManager(), SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentIndex(int)));
	
	for (int i = 0; i < 7; ++i)
	{
		_colorButtons.at(i)->setColor(fsCore()->paletteManager()->color(i));
	}
	setCurrentIndex(fsCore()->paletteManager()->currentIndex());
	
	_colorWheel->setColor(fsCore()->paletteManager()->currentColor());
	_sliderPanel->setColor(fsCore()->paletteManager()->currentColor());
	_webColorPanel->setColor(fsCore()->paletteManager()->currentColor());
	
	connect(_wheelButton, SIGNAL(toggled(bool)), _colorWheel, SLOT(setVisible(bool)));
	connect(_sliderButton, SIGNAL(toggled(bool)), _sliderPanel, SLOT(setVisible(bool)));
	connect(_webButton, SIGNAL(toggled(bool)), _webColorPanel, SLOT(setVisible(bool)));
	
	_wheelButton->setChecked(true);
	
	_colorWheel->setVisible(_wheelButton->isChecked());
	_sliderPanel->setVisible(_sliderButton->isChecked());
	_webColorPanel->setVisible(_webButton->isChecked());
	
	fsApplyMacSmallSize(this);
	
	resize(0, 0);
}

void FSColorPanel::setColor(int index, const MLColor &color)
{
	_colorButtons.at(index)->setColor(color);
}

void FSColorPanel::setCurrentIndex(int index)
{
	if (_colorButtons.at(index)->isChecked())
		return;
	
	_colorButtons.at(index)->setChecked(true);
}

void FSColorPanel::notifyColorButtonPressed()
{
	int index = _colorButtons.indexOf(reinterpret_cast<FSColorButton *>(sender()));
	if (index >= 0)
		fsCore()->paletteManager()->setCurrentIndex(index);
}
