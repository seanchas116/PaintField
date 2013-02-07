#include <QtGui>
#include <Malachite/Division>

#include "colorslider.h"

namespace PaintField
{

using namespace Malachite;

ColorSlider::ColorSlider(QWidget *parent) :
	QWidget(parent),
	_color(Color::white()),
	_component(Color::Red),
	_stepCount(100),
	_mouseTracked(false)
{
	init();
}

ColorSlider::ColorSlider(Color::Component component, int stepCount, QWidget *parent) :
	QWidget(parent),
	_color(Color::white()),
	_component(component),
	_stepCount(stepCount),
	_mouseTracked(false)
{
	init();
}

void ColorSlider::init()
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	
	createNewImage();
	
	setColor(_color);
}

QSize ColorSlider::sizeHint() const
{
	return QSize(0, 2 * BarMargin + BarHeight);
}

void ColorSlider::setComponent(Color::Component component)
{
	_component = component;
	updateImage();
	update();
}

void ColorSlider::setColor(const Color &color)
{
	if (_color == color)
		return;
	
	bool componentChanged = (_color.component(_component) != color.component(_component));
	
	_color = color;
	
	emit colorChanged(color);
	
	if (componentChanged)
	{
		emit valueChanged(value());
		emit value8BitChanged(value8Bit());
	}
	
	updateImage();
	update();
}

void ColorSlider::setValue(double x)
{
	Color color = _color;
	color.setComponent(_component, x);
	setColor(color);
}

void ColorSlider::setValue8Bit(int x)
{
	if (value8Bit() != x)
		setValue(double(x) * (1.0 / 255.0));
}

void ColorSlider::resizeEvent(QResizeEvent *)
{
	createNewImage();
	updateImage();
	update();
}

void ColorSlider::mousePressEvent(QMouseEvent *event)
{
	if (barRect().contains(event->pos()))
	{
		_mouseTracked = true;
		mouseMoveEvent(event);
	}
}

void ColorSlider::mouseMoveEvent(QMouseEvent *event)
{
	double x = (double)(event->x() - BarMargin) / (double)barSize().width();
	x = qBound(0.0, x, 1.0);
	
	Color color = _color;
	
	color.setComponent(_component, Malachite::align(x, 1.0 / (double)_stepCount));
	setColor(color);
}

void ColorSlider::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	_mouseTracked = false;
}

void ColorSlider::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	
	painter.drawImage(barRect().topLeft(), _image);
	
	QRect rect(-2, BarMargin - 1, 4, BarHeight + 2);
	
	double value = _color.component(_component);
	
	painter.setPen(Qt::black);
	painter.setBrush(Qt::white);
	painter.translate(BarMargin + value * barSize().width(), 0);
	painter.setRenderHint(QPainter::Antialiasing, true);
	
	painter.drawRect(rect);
}

void ColorSlider::createNewImage()
{
	_image = QImage(barSize(), QImage::Format_ARGB32_Premultiplied);
}

void ColorSlider::updateImage()
{
	if (_image.isNull())
		return;
	
	int width = _image.width();
	int height = _image.height();
	
	double rwidth = 1.0 / width;
	
	for (int x = 0; x < width; ++x)
	{
		double value = (x + 0.5) * rwidth;
		
		Color color = _color;
		color.setComponent(_component, value);
		
		for (int y = 0; y < height; ++y)
			_image.setPixel(x, y, color.toQRgbPremult());
	}
}

}
