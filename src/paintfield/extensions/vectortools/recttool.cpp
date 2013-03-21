#include <QApplication>
#include "paintfield/core/rectlayer.h"

#include "recttool.h"

using namespace Malachite;

namespace PaintField {

struct RectTool::Data
{
	const RectLayer *layer = 0;
	QScopedPointer<RectLayer> layerToAdd;
	Vec2D start;
	const Layer *parent;
	int index;
	bool isDragged = false;
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
	d->start = event->data.pos;
	d->isDragged = true;
}

void RectTool::tabletMoveEvent(CanvasTabletEvent *event)
{
	auto pos = event->data.pos;
	
	if (d->layerToAdd)
	{
		resizeAddRect(pos);
	}
	else
	{
		if (d->isDragged)
		{
			auto diff = d->start - pos;
			if ((::std::fabs(diff.x()) + ::std::fabs(diff.y())) >= qApp->startDragDistance())
			{
				startAddRect();
			}
		}
	}
}

void RectTool::tabletReleaseEvent(CanvasTabletEvent *event)
{
	Q_UNUSED(event)
	
	d->isDragged = false;
	
	if (d->layerToAdd)
	{
		finishAddRect();
	}
	else
	{
		auto layer = layerModel()->rootLayer()->descendantAt(event->data.pos.toQPoint());
		if (layer)
		{
			auto index = layerModel()->indexForLayer(layer);
			canvas()->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Current);
		}
	}
}

void RectTool::startAddRect()
{
	d->layerToAdd.reset(new RectLayer);
	d->layerToAdd->setFillBrush(Color::fromRgbValue(0.5, 0.5, 0.5));
	d->layerToAdd->setStrokeBrush(Color::fromRgbValue(0, 0, 0));
	
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
	
	addLayerInsertion(d->parent, d->index, d->layerToAdd.data());
}

void RectTool::resizeAddRect(const Vec2D &pos)
{
	auto offset = pos - d->start;
	
	auto oldBoundingRect = d->layerToAdd->boundingRect();
	d->layerToAdd->setRect(QRectF(d->start.x(), d->start.y(), offset.x(), offset.y()));
	
	emit requestUpdate(Surface::rectToKeys((oldBoundingRect | d->layerToAdd->boundingRect()).toAlignedRect()));
}

void RectTool::finishAddRect()
{
	clearLayerInsertions();
	
	auto parentIndex = layerModel()->indexForLayer(d->parent);
	layerModel()->addLayers({d->layerToAdd.take()}, parentIndex, d->index, tr("Add Rectangle"));
	canvas()->selectionModel()->setCurrentIndex(parentIndex.child(d->index, 0), QItemSelectionModel::Current);
}

} // namespace PaintField
