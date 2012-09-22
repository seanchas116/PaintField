#include <QtGui>
#include "colorbutton.h"

namespace PaintField
{

using namespace Malachite;

ColorButton::ColorButton(QWidget *parent) :
	QAbstractButton(parent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setCheckable(true);
}

QSize ColorButton::sizeHint() const
{
	return QSize(ButtonSize + 2 * ButtonMargin, ButtonSize + 2 * ButtonMargin);
}

void ColorButton::setColor(const Color &c)
{
	_color = c;
	update();
}

void ColorButton::paintEvent(QPaintEvent *)
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

}
