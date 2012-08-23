#include <QtGui>

#include "mldivision.h"

#include "fscolorslider.h"

FSColorSlider::FSColorSlider(QWidget *parent) :
	QWidget(parent),
	_color(MLColor::white()),
	_component(MLColor::Red),
	_stepCount(100),
	_mouseTracked(false)
{
	init();
}

FSColorSlider::FSColorSlider(MLColor::Component component, int stepCount, QWidget *parent) :
	QWidget(parent),
	_color(MLColor::white()),
	_component(component),
	_stepCount(stepCount),
	_mouseTracked(false)
{
	init();
}

void FSColorSlider::init()
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	
	createNewImage();
	
	setColor(_color);
}

QSize FSColorSlider::sizeHint() const
{
	return QSize(0, 2 * BarMargin + BarHeight);
}

void FSColorSlider::setComponent(MLColor::Component component)
{
	_component = component;
	updateImage();
	update();
}

void FSColorSlider::setColor(const MLColor &color)
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

void FSColorSlider::setValue(double x)
{
	MLColor color = _color;
	color.setComponent(_component, x);
	setColor(color);
}

void FSColorSlider::setValue8Bit(int x)
{
	setValue(double(x) * (1.0 / 255.0));
}

void FSColorSlider::resizeEvent(QResizeEvent *)
{
	createNewImage();
	updateImage();
	update();
}

void FSColorSlider::mousePressEvent(QMouseEvent *event)
{
	if (barRect().contains(event->pos()))
	{
		_mouseTracked = true;
		mouseMoveEvent(event);
	}
}

void FSColorSlider::mouseMoveEvent(QMouseEvent *event)
{
	double x = (double)(event->x() - BarMargin) / (double)barSize().width();
	x = qBound(0.0, x, 1.0);
	
	
	MLColor color = _color;
	color.setNormalizedComponent(_component, mlAlign(x, 1.0 / (double)_stepCount));
	setColor(color);
}

void FSColorSlider::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	_mouseTracked = false;
}

void FSColorSlider::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	
	painter.drawImage(barRect().topLeft(), _image);
	
	QRect rect(-2, BarMargin - 1, 4, BarHeight + 2);
	
	double value = _color.intervaledComponent(_component);
	
	painter.setPen(Qt::black);
	painter.setBrush(Qt::white);
	painter.translate(BarMargin + value * barSize().width(), 0);
	painter.setRenderHint(QPainter::Antialiasing, true);
	
	painter.drawRect(rect);
}

void FSColorSlider::createNewImage()
{
	_image = QImage(barSize(), QImage::Format_ARGB32_Premultiplied);
}

void FSColorSlider::updateImage()
{
	if (_image.isNull())
		return;
	
	int width = _image.width();
	int height = _image.height();
	
	for (int x = 0; x < width; ++x)
	{
		double value = ((double)x + 0.5) / (double)width;
		
		MLColor color = _color;
		color.setNormalizedComponent(_component, value);
		
		QRgb rgb = color.toQRgb();
		
		for (int y = 0; y < height; ++y)
		{
			_image.setPixel(x, y, rgb);
		}
	}
}

