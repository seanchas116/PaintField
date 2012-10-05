#include "Malachite/mlsurfacepainter.h"
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

FSLayerPropertyEdit::FSLayerPropertyEdit(const QVariant &property, int role) :
      LayerEdit(),
      _newProperty(property),
      _role(role)
{}

void FSLayerPropertyEdit::saveUndoState(const Layer *layer)
{
	_oldProperty = layer->property(_role);
}

void FSLayerPropertyEdit::redo(Layer *layer)
{
	layer->setProperty(_newProperty, _role);
}

void FSLayerPropertyEdit::undo(Layer *layer)
{
	layer->setProperty(_oldProperty, _role);
}

FSLayerSurfaceEdit::FSLayerSurfaceEdit(const Surface &surface, const QPointSet &tileKeys) :
	LayerEdit(),
	_surface(surface)
{
	setModifiedKeys(tileKeys);
}

void FSLayerSurfaceEdit::redo(Layer *layer)
{
	RasterLayer *rasterLayer = dynamic_cast<RasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	
	Surface surface = rasterLayer->surface();
	rasterLayer->setSurface(_surface);
	_surface = surface;
}

void FSLayerSurfaceEdit::undo(Layer *layer)
{
	RasterLayer *rasterLayer = dynamic_cast<RasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	
	Surface surface = rasterLayer->surface();
	rasterLayer->setSurface(_surface);
	_surface = surface;
}

}
