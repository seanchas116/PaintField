#include "paintfield/core/rectlayer.h"

#include "recttool.h"

using namespace Malachite;

namespace PaintField {

struct RectTool::Data
{
	QScopedPointer<RectLayer> layer;
	Vec2D start;
	const Layer *parent;
	int index;
};

RectTool::RectTool(Canvas *canvas) :
	Tool(canvas),
	d(new Data)
{
	
}

RectTool::~RectTool()
{
	delete d;
}

void RectTool::tabletPressEvent(CanvasTabletEvent *event)
{
	d->layer.reset(new RectLayer);
	d->layer->setFillBrush(Color::fromRgbValue(0.5, 0.5, 0.5));
	d->layer->setStrokeBrush(Color::fromRgbValue(0, 0, 0));
	d->start = event->data.pos;
	
	auto current = currentLayer();
	if (current != layerModel()->rootLayer())
	{
		d->parent = current->parent();
		d->index = current->index();
	}
	else
	{
		d->parent = layerModel()->rootLayer();
		d->index = 0;
	}
	
	addLayerInsertion(d->parent, d->index, d->layer.data());
}

void RectTool::tabletMoveEvent(CanvasTabletEvent *event)
{
	if (!d->layer)
		return;
	
	auto offset = event->data.pos - d->start;
	
	auto oldBoundingRect = d->layer->boundingRect();
	d->layer->setRect(QRectF(d->start.x(), d->start.y(), offset.x(), offset.y()));
	
	emit requestUpdate(Surface::rectToKeys((oldBoundingRect | d->layer->boundingRect()).toAlignedRect()));
}

void RectTool::tabletReleaseEvent(CanvasTabletEvent *event)
{
	Q_UNUSED(event)
	
	clearLayerInsertions();
	
	auto parentIndex = layerModel()->indexForLayer(d->parent);
	layerModel()->addLayers({d->layer.take()}, parentIndex, d->index, tr("Add Rectangle"));
	canvas()->selectionModel()->setCurrentIndex(parentIndex.child(d->index, 0), QItemSelectionModel::Current);
}

} // namespace PaintField
