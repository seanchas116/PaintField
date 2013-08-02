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
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_OpaquePaintEvent);
}

void CanvasViewportNormal::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	
	QRect rect = event->rect();
	
	// get rid of unnecessary state change for performance
	if (!(rect & QRect(QPoint(), _state->documentSize)).isEmpty())
	{
		painter.setPen(Qt::NoPen);
		painter.setBrush(QColor(128, 128, 128));
	}
	
	auto draw = [&](const QRect &rect, const QImage &image)
	{
		painter.drawImage(rect, image);
	};
	
	auto drawBackground = [&](const QRect &rect)
	{
		painter.drawRect(rect);
	};
	
	drawViewport(rect, _state, draw, drawBackground);
}

} // namespace PaintField
