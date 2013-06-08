#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>

#include "canvasviewportstate.h"
#include "canvasviewportnormal.h"

using namespace Malachite;

namespace PaintField {

CanvasViewportNormal::CanvasViewportNormal(QWidget *parent) :
    QWidget(parent)
{
}

void CanvasViewportNormal::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(128, 128, 128));
	
	auto draw = [&](const QRect &rect, const QImage &image)
	{
		painter.drawImage(rect, image);
	};
	
	auto drawBackground = [&](const QRect &rect)
	{
		painter.drawRect(rect);
	};
	
	drawViewport(event->rect(), _state, draw, drawBackground);
}

} // namespace PaintField
