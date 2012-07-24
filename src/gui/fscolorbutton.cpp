#include <QtGui>
#include "fscolorbutton.h"

FSColorButton::FSColorButton(QWidget *parent) :
	QAbstractButton(parent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setCheckable(true);
}

QSize FSColorButton::sizeHint() const
{
	return QSize(ButtonSize + 2 * ButtonMargin, ButtonSize + 2 * ButtonMargin);
}

void FSColorButton::setColor(const MLColor &c)
{
	_color = c;
	update();
}

void FSColorButton::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	
	painter.setBrush(_color.toQColor());
	painter.setPen(Qt::NoPen);
	
	QRect buttonRect(ButtonMargin, ButtonMargin, ButtonSize, ButtonSize);
	
	painter.drawRect(buttonRect);
	
	if (isChecked())
	{
		painter.setBrush(Qt::NoBrush);
		//painter.setPen(palette().color(QPalette::Active, QPalette::Highlight));
		painter.setPen(QPen(QColor(39, 96, 209), 2.0));
		painter.setRenderHint(QPainter::Antialiasing, true);
		
		painter.drawRect(buttonRect);
	}
}
