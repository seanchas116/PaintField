#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>

#include "canvasviewportnormal.h"

using namespace Malachite;

namespace PaintField {

CanvasViewportNormal::CanvasViewportNormal(Malachite::SurfaceU8 *surface, QWidget *parent) :
    QWidget(parent),
    _surface(surface)
{
}

void CanvasViewportNormal::paintEvent(QPaintEvent *event)
{
	auto viewRect = event->rect();
	auto sceneRect = _transformToScene.mapRect(viewRect);
	
	ImageU8 image = _repaintImage;
	if (!image.isValid())
		image = _surface->crop<ImageU8>(sceneRect);
	
	QPainter painter(this);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.drawImage(viewRect.topLeft(), image.wrapInQImage());
	
	_repaintImage = ImageU8();
}

} // namespace PaintField
