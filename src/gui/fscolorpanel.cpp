#include <QtGui>
#include "fscore.h"
#include "fscolorbutton.h"
#include "fscolorslider.h"
#include "fscolorwheel.h"
#include "fsdoubleedit.h"
#include "fssimplebutton.h"
#include "fswidgetgroup.h"

#include "fscolorpanel.h"


inline void fsConnectMutual(QObject *object1, const char *signal, QObject *object2, const char *slot)
{
	QObject::connect(object1, signal, object2, slot);
	QObject::connect(object2, signal, object1, slot);
}

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
	
	_sliders << new FSColorSlider(MLColor::Red, 1000);
	_sliders << new FSColorSlider(MLColor::Green, 1000);
	_sliders << new FSColorSlider(MLColor::Blue, 10000);
	_sliders << new FSColorSlider(MLColor::Hue, 3600);
	_sliders << new FSColorSlider(MLColor::Saturation, 1000);
	_sliders << new FSColorSlider(MLColor::Value, 1000);
	
	for (int i = 0; i < 6; ++i)
	{
		fsConnectMutual(_sliders.at(i), SIGNAL(colorChanged(MLColor)), this, SLOT(setColor(MLColor)));
		fsConnectMutual(_sliders.at(i), SIGNAL(valueChanged(double)), _lineEdits.at(i), SLOT(setValue(double)));
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
	
	connect(_comboBox, SIGNAL(activated(int)), this, SLOT(onComboBoxActivated(int)));
	
	_comboBox->setCurrentIndex(0);
	onComboBoxActivated(0);
	
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
	
	setColor(MLColor::white());
	
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
}

void FSColorSliderPanel::setColor(const MLColor &color)
{
	if (_color == color)
		return;
	
	_color = color;
	
	emit colorChanged(color);
}

void FSColorSliderPanel::onComboBoxActivated(int index)
{
	switch (index)
	{
	case 0:
		for (int i = 3; i < 6; ++i)
		{
			_labels.at(i)->setVisible(false);
			_sliders.at(i)->setVisible(false);
			_lineEdits.at(i)->setVisible(false);
		}
		for (int i = 0; i < 3; ++i)
		{
			_labels.at(i)->setVisible(true);
			_sliders.at(i)->setVisible(true);
			_lineEdits.at(i)->setVisible(true);
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
	
	connect(_lineEdit, SIGNAL(editingFinished()), this, SLOT(onLineEditEditingFinished()));
}

void FSWebColorPanel::setColor(const MLColor &color)
{
	if (_color == color)
		return;
	
	_color = color;
	_lineEdit->setText(color.toWebColor());
	
	emit colorChanged(color);
}

void FSWebColorPanel::onLineEditEditingFinished()
{
	bool ok;
	MLColor newColor = MLColor::fromWebColor(_lineEdit->text(), &ok);
	
	if (ok)
	{
		setColor(newColor);
	}
	else
	{
		_lineEdit->setText(color().toWebColor());
	}
}

FSColorPanel::FSColorPanel(QWidget *parent) :
	QWidget(parent)
{
	setWindowTitle(tr("Color"));
	
	
	// buttons
	
	_buttonLayout = new QHBoxLayout();
	
	_wheelButton = new FSSimpleButton(":/icons/24x24/colorWheel.svg");
	_sliderButton = new FSSimpleButton(":/icons/24x24/colorSlider.svg");
	_webButton = new FSSimpleButton(":/icons/24x24/webColor.svg");
	_dialogButton = new FSSimpleButton(":/icons/24x24/colorDialog.svg");
	
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
		button->setColor(MLColor::white());
		_colorButtons << button;
		_colorButtonLayout->addWidget(button);
		_colorButtonGroup->addButton(button);
		connect(button, SIGNAL(pressed()), this, SLOT(onColorButtonPressed()));
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
	
	//fsApplyMacSmallSize(this);
	
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

void FSColorPanel::onColorButtonPressed()
{
	int index = _colorButtons.indexOf(reinterpret_cast<FSColorButton *>(sender()));
	if (index >= 0)
		fsCore()->paletteManager()->setCurrentIndex(index);
}
