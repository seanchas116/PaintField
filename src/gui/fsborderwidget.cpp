#include <QtGui>

#include "fsborderwidget.h"

FSBorderWidget::FSBorderWidget(QWidget *parent) :
    QWidget(parent),
    _height(1),
    _color(180, 180, 180)
{
	updateHeight();
}

void FSBorderWidget::paintEvent(QPaintEvent *)
{
	QRect rect(_margins.left(), _margins.top(), width() - _margins.left() - _margins.right(), _height);
	QPainter painter(this);
	painter.setBrush(_color);
	painter.setPen(Qt::NoPen);
	painter.drawRect(rect);
}

void FSBorderWidget::updateHeight()
{
	int height = _margins.top() + _height + _margins.bottom();
	setMaximumHeight(height);
	setMinimumHeight(height);
}
