#include <QVBoxLayout>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QHBoxLayout>

#include <Malachite/Division>
#include <Malachite/Affine2D>
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/settingsmanager.h"
#include "paintfield/core/widgets/simplebutton.h"
#include "paintfield/core/widgets/looseslider.h"
#include "paintfield/core/widgets/loosespinbox.h"
#include "paintfield/core/signalconverter.h"
#include "paintfield/core/canvas.h"

#include "minimapview.h"

#include "navigatorview.h"

using namespace Malachite;

namespace PaintField {

NavigatorView::NavigatorView(Canvas *canvas, QWidget *parent) :
    QWidget(parent)
{
	connect(this, SIGNAL(translationChanged(QPoint)), this, SLOT(onTranslationChanged(QPoint)));
	this->createWidgets(canvas);

	if (canvas)
	{
		connect(this, SIGNAL(scaleChanged(double)), canvas, SLOT(setScale(double)));
		connect(canvas, SIGNAL(scaleChanged(double)), this, SLOT(setScale(double)));

		connect(this, SIGNAL(rotationChanged(double)), canvas, SLOT(setRotation(double)));
		connect(canvas, SIGNAL(rotationChanged(double)), this, SLOT(setRotation(double)));

		connect(this, SIGNAL(translationChanged(QPoint)), canvas, SLOT(setTranslation(QPoint)));
		connect(canvas, SIGNAL(translationChanged(QPoint)), this, SLOT(setTranslation(QPoint)));

		connect(this, SIGNAL(mirroringEnabledChanged(bool)), canvas, SLOT(setMirrored(bool)));
		connect(canvas, SIGNAL(mirroredChanged(bool)), this, SLOT(setMirroringEnabled(bool)));

		connect(this, SIGNAL(retinaModeChanged(bool)), canvas, SLOT(setRetinaMode(bool)));
		connect(canvas, SIGNAL(retinaModeChanged(bool)), this, SLOT(setRetinaMode(bool)));

		this->setTranslation(canvas->translation());
		this->setScale(canvas->scale());
		this->setRotation(canvas->rotation());
		this->setMirroringEnabled(canvas->isMirrored());
	}
	else
	{
		this->setEnabled(false);
	}
}

void NavigatorView::setScale(double scale)
{
	scale = qBound(scaleMin(), scale, scaleMax());
	
	if (_scale != scale)
	{
		_scale = scale;
		_currentMode = Translation;
		emit scaleChanged(scale);
	}
}

void NavigatorView::setRotation(double rotation)
{
	rotation = Malachite::Division(rotation + 180.0, 360.0).rem() - 180.0;
	
	if (rotation == -180.0)
		rotation += 360.0;
	
	if (_rotationD != rotation)
	{
		_rotationD = rotation;
		_currentMode = Translation;
		emit rotationChanged(rotation);
	}
}

void NavigatorView::setViewScale(double scale)
{
	scale = qBound(scaleMin(), scale, scaleMax());
	
	if (_scale != scale)
	{
		if (_currentMode != Scaling)
			setOriginalValues();
		
		_scale = scale;
		emit scaleChanged(scale);
		
		_translation = (Vec2D(_originalTranslation) * (scale / _originalScale)).toQPoint();
		emit translationChanged(_translation);
	}
}

void NavigatorView::setViewRotation(double rotation)
{
	rotation = Malachite::Division(rotation + 180.0, 360.0).rem() - 180.0;
	
	if (rotation == -180.0)
		rotation += 360.0;
	
	if (_rotationD != rotation)
	{
		if (_currentMode != Rotation)
			setOriginalValues();
		
		_rotationD = rotation;
		emit rotationChanged(rotation);
		
		_translation = (Affine2D::fromRotationDegrees(rotation - _originalRotation) * Vec2D(_originalTranslation)).toQPoint();
		emit translationChanged(_translation);
	}
}

void NavigatorView::setTranslation(const QPoint &value)
{
	if (_translation != value)
	{
		_translation = value;
		_currentMode = Translation;
		emit translationChanged(value);
	}
}

void NavigatorView::setMirroringEnabled(bool enabled)
{
	if (_mirrorOn != enabled)
	{
		_mirrorOn = enabled;
		emit mirroringEnabledChanged(enabled);
	}
}

void NavigatorView::setRetinaMode(bool mode)
{
	if (_retinaMode != mode)
	{
		_retinaMode = mode;
		emit retinaModeChanged(mode);
	}
}

void NavigatorView::onTranslationChanged(const QPoint &translation)
{
	emit translationXChanged(translation.x());
	emit translationYChanged(translation.y());
}

void NavigatorView::createWidgets(Canvas *canvas)
{
	auto mainLayout = new QVBoxLayout;
	mainLayout->addWidget(new MinimapView(canvas));
	mainLayout->addLayout(createScaleRotationUILayout());
	mainLayout->addLayout(createMiscUILayout());
	
	{
		auto checkBoxLayout = new QVBoxLayout;
		checkBoxLayout->setSpacing(12);
		
		{
			auto checkBox = new QCheckBox(tr("Mirror"));
			checkBox->setToolTip(tr("Mirror horizontally"));
			connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(setMirroringEnabled(bool)));
			connect(this, SIGNAL(mirroringEnabledChanged(bool)), checkBox, SLOT(setChecked(bool)));
			
			checkBoxLayout->addWidget(checkBox);
		}
		
		{
			auto checkBox = new QCheckBox(tr("Retina Mode"));
			checkBox->setToolTip(tr("Display canvas suitablly for Retina display Macs (not suitable for non-Retina displays)"));
			connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(setRetinaMode(bool)));
			connect(this, SIGNAL(retinaModeChanged(bool)), checkBox, SLOT(setChecked(bool)));
			
			checkBoxLayout->addWidget(checkBox);
		}
		
		mainLayout->addLayout(checkBoxLayout);
	}
	
	// key binding descriptions
	{
		auto bindingMap = appController()->settingsManager()->value({".key-bindings"}).toMap();
		
		auto translationKeys = bindingMap["paintfield.canvas.dragTranslation"].toString();
		auto scaleKeys = bindingMap["paintfield.canvas.dragScale"].toString();
		auto rotationKeys = bindingMap["paintfield.canvas.dragRotation"].toString();
		
		auto dragText = tr("Drag");
		
		{
			auto label = new QLabel("<b>" + translationKeys + "+" + dragText + "</b> " +  tr("to translate"));
			mainLayout->addWidget(label);
		}
		
		{
			auto label = new QLabel("<b>" + scaleKeys + "+" + dragText + "</b> " +  tr("to scale"));
			mainLayout->addWidget(label);
		}
		
		{
			auto label = new QLabel("<b>" + rotationKeys + "+" + dragText + "</b> " +  tr("to rotate"));
			mainLayout->addWidget(label);
		}
	}
	
	mainLayout->addStretch(1);
	
	setLayout(mainLayout);
}

QLayout *NavigatorView::createScaleRotationUILayout()
{
	auto layout = new QGridLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	
	// row 0 (scale)
	
	auto iconSize = QSize(16, 16);
	
	{
		// zoom out
		auto button = new SimpleButton(":/icons/16x16/zoomOut.svg", iconSize, this, SLOT(zoomOut()));
		layout->addWidget(button, 0, 0);
	}
	
	{
		// scale slider
		
		constexpr int resolution = 32;
		
		// slider value: log2(scale()) * resolution
		auto slider = new QSlider(Qt::Horizontal);
		slider->setRange(_scaleLogMin * resolution, _scaleLogMax * resolution);
		slider->setValue(0);
		
		auto to = [](const QVariant &normalValue)
		{
			return qRound(log2(normalValue.toDouble()) * resolution);
		};
		
		auto from = [](const QVariant &intLogValue)
		{
			return exp2(intLogValue.toDouble() / resolution);
		};
		
		auto signalConverter = new SignalConverter(to, from, this);
		signalConverter->connectChannelADouble(this, SIGNAL(scaleChanged(double)), SLOT(setViewScale(double)));
		signalConverter->connectChannelBInt(slider, SIGNAL(valueChanged(int)), SLOT(setValue(int)));
		
		layout->addWidget(slider, 0, 1);
	}
	
	{
		// zoom out
		auto button = new SimpleButton(":/icons/16x16/zoomIn.svg", iconSize, this, SLOT(zoomIn()));
		layout->addWidget(button, 0, 2);
	}
	
	{
		// revert scale
		auto button = new SimpleButton(":/icons/16x16/revert.svg", iconSize, this, SLOT(resetScale()));
		layout->addWidget(button, 0, 3);
	}
	
	{
		// scale spin box (percentage)
		
		auto spinBox = new LooseSpinBox;
		spinBox->setRange(scaleMin() * 100.0, scaleMax() * 100.0);
		spinBox->setDecimals(1);
		spinBox->setMinimumWidth(_spinBoxWidth);
		spinBox->setMaximumWidth(_spinBoxWidth);
		spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		spinBox->setAlignment(Qt::AlignRight);
		spinBox->setValue(100);
		
		auto toPercent = [](const QVariant &value) { return value.toDouble() * 100.0; };
		auto fromPercent = [](const QVariant &value) { return value.toDouble() / 100.0; };
		
		auto signalConverter = new SignalConverter(toPercent, fromPercent, this);
		signalConverter->connectChannelADouble(this, SIGNAL(scaleChanged(double)), SLOT(setViewScale(double)));
		signalConverter->connectChannelBDouble(spinBox, SIGNAL(valueChanged(double)), SLOT(setValue(double)));
		
		layout->addWidget(spinBox, 0, 4);
	}
	
	layout->addWidget(new QLabel("%"), 0, 5);
	
	// row 1 (rotation)
	
	{
		// rotate left
		auto button = new SimpleButton(":/icons/16x16/rotateLeft.svg", iconSize, this, SLOT(rotateLeft()));
		layout->addWidget(button, 1, 0);
	}
	
	{
		// rotation slider
		
		auto slider = new QSlider(Qt::Horizontal);
		slider->setRange(-180, 180);
		slider->setValue(0);
		
		auto toInt = [](const QVariant &doubleValue)
		{
			return qRound(doubleValue.toDouble());
		};
		
		auto toDouble = [](const QVariant &intValue)
		{
			return intValue.toDouble();
		};
		
		auto signalConverter = new SignalConverter(toInt, toDouble, this);
		signalConverter->connectChannelADouble(this, SIGNAL(rotationChanged(double)), SLOT(setViewRotation(double)));
		signalConverter->connectChannelBInt(slider, SIGNAL(valueChanged(int)), SLOT(setValue(int)));
		
		layout->addWidget(slider, 1, 1);
	}
	
	{
		// rotate right
		auto button = new SimpleButton(":/icons/16x16/rotateRight.svg", iconSize, this, SLOT(rotateRight()));
		layout->addWidget(button, 1, 2);
	}
	
	{
		// zoom out
		auto button = new SimpleButton(":/icons/16x16/revert.svg", iconSize, this, SLOT(resetRotation()));
		layout->addWidget(button, 1, 3);
	}
	
	{
		// rotation spin box
		
		auto spinBox = new LooseSpinBox;
		spinBox->setValue(0);
		spinBox->setRange(INT_MIN, INT_MAX);
		spinBox->setMinimumWidth(_spinBoxWidth);
		spinBox->setMaximumWidth(_spinBoxWidth);
		spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		spinBox->setAlignment(Qt::AlignRight);
		
		connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(setViewRotation(double)));
		connect(this, SIGNAL(rotationChanged(double)), spinBox, SLOT(setValue(double)));
		
		layout->addWidget(spinBox, 1, 4);
	}
	
	layout->addWidget(new QLabel("°"), 1, 5);
	
	return layout;
}

QLayout *NavigatorView::createMiscUILayout()
{
	auto layout = new QHBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	
	layout->addWidget(new QLabel(tr("x:")));
	
	{
		// x spinbox
		
		auto spinBox = new QSpinBox;
		spinBox->setRange(INT_MIN, INT_MAX);
		spinBox->setMinimumWidth(_spinBoxWidth);
		spinBox->setMaximumWidth(_spinBoxWidth);
		spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		spinBox->setAlignment(Qt::AlignRight);
		spinBox->setKeyboardTracking(false);
		
		connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(setTranslationX(int)));
		connect(this, SIGNAL(translationXChanged(int)), spinBox, SLOT(setValue(int)));
		
		layout->addWidget(spinBox);
	}
	
	layout->addWidget(new QLabel(tr("y:")));
	
	{
		// y spinbox
		
		auto spinBox = new QSpinBox;
		spinBox->setRange(INT_MIN, INT_MAX);
		spinBox->setMinimumWidth(_spinBoxWidth);
		spinBox->setMaximumWidth(_spinBoxWidth);
		spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		spinBox->setAlignment(Qt::AlignRight);
		spinBox->setKeyboardTracking(false);
		
		connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(setTranslationY(int)));
		connect(this, SIGNAL(translationYChanged(int)), spinBox, SLOT(setValue(int)));
		
		layout->addWidget(spinBox);
	}
	
	{
		// reset translation
		auto button = new SimpleButton(":/icons/16x16/revert.svg", QSize(16,16), this, SLOT(resetTranslation()));
		layout->addWidget(button);
	}
	
	layout->addStretch(1);
	
	return layout;
}

void NavigatorView::setOriginalValues()
{
	_originalTranslation = _translation;
	_originalScale = _scale;
	_originalRotation = _rotationD;
}


}
