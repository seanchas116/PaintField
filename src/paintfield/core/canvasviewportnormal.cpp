#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>

#include "canvasviewportcontroller.h"
#include "canvasviewportnormal.h"

using namespace Malachite;

namespace PaintField {

CanvasViewportNormal::CanvasViewportNormal(CanvasViewportSurface *surface, QWidget *parent) :
    QWidget(parent),
    _surface(surface)
{
}

void CanvasViewportNormal::paintEvent(QPaintEvent *event)
{
	auto viewRect = event->rect();
	QPainter painter(this);
	
	auto draw = [&](const QRect &view)
	{
		auto sceneRect = _transformToScene.mapRect(viewRect);
		ImageU8 image = _surface->crop<ImageU8>(sceneRect);
		
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		painter.drawImage(viewRect.topLeft(), image.wrapInQImage());
	};
	
	drawDivided(viewRect, draw);
}

} // namespace PaintField
