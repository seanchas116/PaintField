#include <QtGui>
#include "paintfield-core/widgets/simplebutton.h"
#include "paintfield-core/widgets/doubleslider.h"
#include "paintfield-core/widgets/loosespinbox.h"
#include "paintfield-core/signalconverter.h"

#include "navigatorview.h"

namespace PaintField {

NavigatorView::NavigatorView(QWidget *parent) :
    QWidget(parent)
{
	createWidgets();
}

void NavigatorView::setScale(double scale)
{
	scale = qBound(exp2(_scaleLogMin), scale, exp2(_scaleLogMax));
	
	if (_scale != scale)
	{
		_scale = scale;
		emit scaleChanged(scale);
	}
}

void NavigatorView::setRotation(int rotation)
{
	rotation = qBound(_rotationMin, rotation, _rotationMax);
	
	if (_rotation != rotation)
	{
		_rotation = rotation;
		emit rotationChanged(rotation);
	}
}

void NavigatorView::setTranslation(const QPoint &value)
{
	if (_translation != value)
	{
		_translation = value;
		
		emit translationChanged(value);
		emit xChanged(value.x());
		emit yChanged(value.y());
	}
}

void NavigatorView::createWidgets()
{
	auto mainLayout = new QVBoxLayout;
	mainLayout->addLayout(createScaleRotationUILayout());
	mainLayout->addLayout(createMiscUILayout());
	
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
		slider->setMinimum(_scaleLogMin * resolution);
		slider->setMaximum(_scaleLogMax * resolution);
		slider->setValue(1 * resolution);
		
		auto to = [](const QVariant &normalValue)
		{
			return log2(normalValue.toDouble()) * resolution;
		};
		
		auto from = [](const QVariant &intLogValue)
		{
			return exp2(intLogValue.toDouble() / resolution);
		};
		
		auto signalConverter = new SignalConverter(to, from, this);
		signalConverter->connectChannelADouble(this, SIGNAL(scaleChanged(double)), SLOT(setScale(double)));
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
		auto button = new SimpleButton(":/icons/16x16/revert.svg", iconSize, this, SLOT(revertScale()));
		layout->addWidget(button, 0, 3);
	}
	
	{
		// scale spin box (percentage)
		
		auto spinBox = new LooseSpinBox;
		spinBox->setValue(100);
		spinBox->setMinimum(scaleMin() * 100);
		spinBox->setMaximum(scaleMax() * 100);
		spinBox->setDecimals(1);
		spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		
		auto toPercent = [](const QVariant &value) { return value.toDouble() * 100.0; };
		auto fromPercent = [](const QVariant &value) { return value.toDouble() / 100.0; };
		
		auto signalConverter = new SignalConverter(toPercent, fromPercent, this);
		signalConverter->connectChannelADouble(this, SIGNAL(scaleChanged(double)), SLOT(setScale(double)));
		signalConverter->connectChannelBDouble(spinBox, SIGNAL(valueChanged(double)), SLOT(setValue(double)));
		
		layout->addWidget(spinBox, 0, 4);
	}
	
	// row 1 (rotation)
	
	{
		// rotate left
		auto button = new SimpleButton(":/icons/16x16/rotateLeft.svg", iconSize, this, SLOT(rotateLeft()));
		layout->addWidget(button, 1, 0);
	}
	
	{
		// rotation slider
		
		auto slider = new QSlider(Qt::Horizontal);
		slider->setMinimum(_rotationMin);
		slider->setMaximum(_rotationMax);
		slider->setValue(0);
		
		connect(this, SIGNAL(rotationChanged(int)), slider, SLOT(setValue(int)));
		connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setRotation(int)));
		
		layout->addWidget(slider, 1, 1);
	}
	
	{
		// rotate right
		auto button = new SimpleButton(":/icons/16x16/rotateRight.svg", iconSize, this, SLOT(rotateRight()));
		layout->addWidget(button, 1, 2);
	}
	
	{
		// zoom out
		auto button = new SimpleButton(":/icons/16x16/revert.svg", iconSize, this, SLOT(revertRotation()));
		layout->addWidget(button, 1, 3);
	}
	
	{
		// rotation spin box (percentage)
		
		auto spinBox = new LooseSpinBox;
		spinBox->setValue(0);
		spinBox->setMaximum(180);
		spinBox->setMinimum(-180);
		spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		
		auto to = [](const QVariant &normalValue)
		{
			return normalValue.toDouble() / 16.0;
		};
		
		auto from = [](const QVariant &spinBoxValue)
		{
			return round(spinBoxValue.toDouble() * 16.0);
		};
		
		auto signalConverter = new SignalConverter(to, from, this);
		signalConverter->connectChannelAInt(this, SIGNAL(rotationChanged(int)), SLOT(setRotation(int)));
		signalConverter->connectChannelBDouble(spinBox, SIGNAL(valueChanged(double)), SLOT(setValue(double)));
		
		layout->addWidget(spinBox, 1, 4);
	}
	
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
		connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(setX(int)));
		connect(this, SIGNAL(xChanged(int)), spinBox, SLOT(setValue(int)));
		spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		
		layout->addWidget(spinBox);
	}
	
	layout->addWidget(new QLabel(tr("y:")));
	
	{
		// y spinbox
		
		auto spinBox = new QSpinBox;
		connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(setY(int)));
		connect(this, SIGNAL(yChanged(int)), spinBox, SLOT(setValue(int)));
		spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		
		layout->addWidget(spinBox);
	}
	
	layout->addStretch(1);
	
	return layout;
}


}
