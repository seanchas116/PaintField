#include <QtGui>

#include "mlcolor.h"
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
			double degree = atan2(pos.y(), pos.x()) / M_PI * 180.0;
			if (degree < 0)
				degree += 360.0;
			
			*p++ = Color::fromHsvValue(degree, 1, 1).toQRgb();
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
	double hue = atan2(-realPos.y(), realPos.x()) / M_PI * 180;
	if (hue < 0.0)
		hue += 360.0;
	
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
	for (int y = 0; y < CenterSquareRadius * 2; ++y)
	{
		QRgb *p = reinterpret_cast<QRgb *>(_hueSelectorImage.scanLine(y + OuterCurcleRadius - CenterSquareRadius)) + OuterCurcleRadius - CenterSquareRadius;
		
		for (int x = 0; x < CenterSquareRadius * 2; ++x)
		{
			double s = (double)x / (double)(CenterSquareRadius * 2 + 1);
			double v = 1.0 - (double)y / (double)(CenterSquareRadius * 2 + 1);
			
			*p++ = Color::fromHsvValue(_color.hue(), s, v).toQRgb();
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
	QPixmap pixmap(OuterCurcleRadius * 2 + 10, OuterCurcleRadius * 2 + 10);
	pixmap.fill(Qt::transparent);
	QPainter pixmapPainter(&pixmap);
	pixmapPainter.translate(OuterCurcleRadius + 5, OuterCurcleRadius + 5);
	
	QPainterPath path;
	path.addEllipse(QPointF(), OuterCurcleRadius, OuterCurcleRadius);
	path.addEllipse(QPointF(), InnerCurcleRadius, InnerCurcleRadius);
	path.addRect(-CenterSquareRadius, -CenterSquareRadius, CenterSquareRadius * 2, CenterSquareRadius * 2);
	
	pixmapPainter.setRenderHint(QPainter::Antialiasing, true);
	
	pixmapPainter.setBrush(Qt::white);
	pixmapPainter.setPen(Qt::NoPen);
	
	pixmapPainter.drawPath(path);
	
	pixmapPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	pixmapPainter.drawImage(-OuterCurcleRadius, -OuterCurcleRadius, _hueSelectorImage);
	
	/*
	pixmapPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	pixmapPainter.setPen(Qt::black);
	pixmapPainter.setBrush(Qt::NoBrush);
	pixmapPainter.drawEllipse(QPointF(), OuterCurcleRadius, OuterCurcleRadius);
	pixmapPainter.drawEllipse(QPointF(), InnerCurcleRadius, InnerCurcleRadius);
	pixmapPainter.setRenderHint(QPainter::Antialiasing, false);
	pixmapPainter.drawRect(-CenterSquareRadius - 1, -CenterSquareRadius - 1, CenterSquareRadius * 2 + 1, CenterSquareRadius * 2 + 1);
	*/
	
	pixmapPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	pixmapPainter.setRenderHint(QPainter::Antialiasing, true);
	pixmapPainter.setBrush(Qt::white);
	pixmapPainter.setPen(Qt::black);
	
	pixmapPainter.drawEllipse(-QPointF(CenterSquareRadius, CenterSquareRadius) + QPointF(_color.saturation(), 1.0 - _color.value()) * (CenterSquareRadius * 2), 3.0, 3.0);
	pixmapPainter.rotate(-_color.hue());
	pixmapPainter.drawRect(InnerCurcleRadius, -2, OuterCurcleRadius - InnerCurcleRadius, 4);
	
	pixmapPainter.end();
	
	QPainter painter(this);
	painter.drawPixmap(width() / 2 - pixmap.width() / 2, height() / 2 - pixmap.height() / 2, pixmap);
}

}
