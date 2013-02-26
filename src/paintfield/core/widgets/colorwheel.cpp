#include <QMouseEvent>
#include <QPainter>
#include <Malachite/Vec2D>

#include "colorwheel.h"

namespace PaintField
{

using namespace Malachite;

ColorWheel::ColorWheel(QWidget *parent) :
	QWidget(parent),
	_wheelBeingDragged(false),
	_squareBeingDragged(false),
	_hueSelectorImage(OuterCurcleRadius * 2, OuterCurcleRadius * 2, QImage::Format_ARGB32_Premultiplied)
{
	for (int y = 0; y < OuterCurcleRadius * 2; ++y)
	{
		QRgb *p = reinterpret_cast<QRgb *>(_hueSelectorImage.scanLine(y));
		
		for (int x = 0; x < OuterCurcleRadius * 2; ++x)
		{
			QPoint pos(x - OuterCurcleRadius + 0.5, -(y - OuterCurcleRadius + 0.5));
			*p++ = Color::fromHsvValue(Vec2D(pos).arg() * 0.5 / M_PI, 1, 1).toQRgbPremult();
		}
	}
	
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	
	updateHsv();
}

QSize ColorWheel::sizeHint() const
{
	return QSize(2 * OuterCurcleRadius + 2, 2 * OuterCurcleRadius + 2);
}

void ColorWheel::setColor(const Color &color)
{
	if (_color == color)
		return;
	
	_color = color;
	
	updateHsv();
}

QPainterPath ColorWheel::wheel() const
{
	QPainterPath path;
	path.addEllipse(center(), OuterCurcleRadius, OuterCurcleRadius);
	path.addEllipse(center(), InnerCurcleRadius, InnerCurcleRadius);
	
	return path;
}

void ColorWheel::calculateH(const QPointF &pos)
{
	QPointF realPos = pos - QPointF(center()) + QPointF(0.5, 0.5);
	
	double hue = Vec2D(realPos.x(), -realPos.y()).arg() * 0.5 / M_PI;
	_color.setHue(hue);
	
	updateHsv();
}

void ColorWheel::calculateSV(const QPointF &pos)
{
	QPointF realPos = pos - center() + QPointF(CenterSquareRadius, CenterSquareRadius);
	
	_color.setSaturation(realPos.x() / (double)(CenterSquareRadius * 2));
	_color.setValue(1.0 - realPos.y() / (double)(CenterSquareRadius * 2));
	
	updateHsv();
}

void ColorWheel::updateHsv()
{
	double factor = 1.0 / double(CenterSquareRadius * 2 - 1);
	
	for (int y = 0; y < CenterSquareRadius * 2; ++y)
	{
		QRgb *p = reinterpret_cast<QRgb *>(_hueSelectorImage.scanLine(y + OuterCurcleRadius - CenterSquareRadius)) + OuterCurcleRadius - CenterSquareRadius;
		
		for (int x = 0; x < CenterSquareRadius * 2; ++x)
		{
			double s = x * factor;
			double v = 1.0 - y * factor;
			
			*p++ = Color::fromHsvValue(_color.hue(), s, v).toQRgbPremult();
		}
	}
	
	update();
	
	emit colorChanged(_color);
}

void ColorWheel::mousePressEvent(QMouseEvent *event)
{
	if (wheel().contains(event->posF()))
		_wheelBeingDragged = true;
	if (square().contains(event->pos()))
		_squareBeingDragged = true;
	
	mouseMoveEvent(event);
}

void ColorWheel::mouseMoveEvent(QMouseEvent *event)
{
	if (_wheelBeingDragged)
		calculateH(event->posF());
	if (_squareBeingDragged)
		calculateSV(event->pos());
}

void ColorWheel::mouseReleaseEvent(QMouseEvent *)
{
	_wheelBeingDragged = false;
	_squareBeingDragged = false;
}

void ColorWheel::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.translate(width()/2, height()/2);
	painter.setRenderHint(QPainter::Antialiasing, true);
	
	{
		QPainterPath path;
		path.addEllipse(QPointF(), OuterCurcleRadius, OuterCurcleRadius);
		path.addEllipse(QPointF(), InnerCurcleRadius, InnerCurcleRadius);
		path.addRect(-CenterSquareRadius, -CenterSquareRadius, CenterSquareRadius * 2, CenterSquareRadius * 2);
		
		QBrush brush(_hueSelectorImage);
		brush.setTransform(QTransform::fromTranslate(-OuterCurcleRadius, -OuterCurcleRadius));
		
		painter.setBrush(brush);
		painter.setPen(Qt::NoPen);
		
		painter.drawPath(path);
	}
	
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setBrush(Qt::white);
	painter.setPen(Qt::black);
	
	painter.drawEllipse(-QPointF(CenterSquareRadius, CenterSquareRadius) + QPointF(_color.saturation(), 1.0 - _color.value()) * (CenterSquareRadius * 2), 3.0, 3.0);
	painter.rotate(-_color.hue() * 360.0);
	painter.drawRect(InnerCurcleRadius, -2, OuterCurcleRadius - InnerCurcleRadius, 4);
}

}
