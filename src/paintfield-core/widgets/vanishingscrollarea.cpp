#include "vanishingscrollbar.h"

#include "vanishingscrollarea.h"

namespace PaintField {

struct VanishingScrollArea::Data
{
	VanishingScrollBar *barX, *barY;
};

VanishingScrollArea::VanishingScrollArea(QWidget *parent) :
	QWidget(parent),
	d(new Data)
{
	d->barX = new VanishingScrollBar(Qt::Horizontal, this);
	d->barY = new VanishingScrollBar(Qt::Vertical, this);
}

VanishingScrollArea::~VanishingScrollArea()
{
	delete d;
}

VanishingScrollBar *VanishingScrollArea::scrollBarX()
{
	return d->barX;
}

VanishingScrollBar *VanishingScrollArea::scrollBarY()
{
	return d->barY;
}

void VanishingScrollArea::moveScrollBars()
{
	int barWidthX = d->barX->totalBarWidth();
	int barWidthY = d->barY->totalBarWidth();
	
	auto widgetRect = QRect(QPoint(), geometry().size());
	
	auto scrollBarXRect = widgetRect.adjusted(0, widgetRect.height() - barWidthY, -barWidthX, 0);
	auto scrollBarYRect = widgetRect.adjusted(widgetRect.width() - barWidthX, 0, 0, -barWidthY);
	
	d->barX->setGeometry(scrollBarXRect);
	d->barY->setGeometry(scrollBarYRect);
}

void VanishingScrollArea::resizeEvent(QResizeEvent *event)
{
	moveScrollBars();
}

} // namespace PaintField
