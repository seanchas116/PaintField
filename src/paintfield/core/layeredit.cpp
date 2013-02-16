#include <Malachite/SurfacePainter>

#include "layer.h"

#include "layeredit.h"

namespace PaintField
{

using namespace Malachite;

void LayerEdit::saveUndoState(const Layer */*layer*/)
{}

void LayerEdit::redo(Layer */*layer*/)
{}

void LayerEdit::undo(Layer */*layer*/)
{}

LayerPropertyEdit::LayerPropertyEdit(const QVariant &property, int role) :
      LayerEdit(),
      _newProperty(property),
      _role(role)
{}

void LayerPropertyEdit::saveUndoState(const Layer *layer)
{
	_oldProperty = layer->property(_role);
}

void LayerPropertyEdit::redo(Layer *layer)
{
	layer->setProperty(_newProperty, _role);
}

void LayerPropertyEdit::undo(Layer *layer)
{
	layer->setProperty(_oldProperty, _role);
}

LayerSurfaceEdit::LayerSurfaceEdit(const Surface &surface, const QPointSet &tileKeys) :
	LayerEdit(),
	_surface(surface)
{
	setModifiedKeys(tileKeys);
}

void LayerSurfaceEdit::redo(Layer *layer)
{
	RasterLayer *rasterLayer = dynamic_cast<RasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	
	Surface surface = rasterLayer->surface();
	rasterLayer->setSurface(_surface);
	_surface = surface;
}

void LayerSurfaceEdit::undo(Layer *layer)
{
	RasterLayer *rasterLayer = dynamic_cast<RasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	
	Surface surface = rasterLayer->surface();
	rasterLayer->setSurface(_surface);
	_surface = surface;
}

}
