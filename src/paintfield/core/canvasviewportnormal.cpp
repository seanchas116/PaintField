#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>

#include "canvasviewportcontroller.h"
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
	
	auto draw = [&](const QRect &rect, const QImage &image)
	{
		painter.drawImage(rect.topLeft(), image);
	};
	
	drawViewport(event->rect(), _state, draw);
}

} // namespace PaintField
