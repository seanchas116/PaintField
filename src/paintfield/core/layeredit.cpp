#include <Malachite/SurfacePainter>

#include "layer.h"
#include "rasterlayer.h"

#include "layeredit.h"

namespace PaintField
{

using namespace Malachite;

void LayerEdit::redo(Layer */*layer*/)
{}

void LayerEdit::undo(Layer */*layer*/)
{}

LayerPropertyEdit::LayerPropertyEdit(const Layer *layer, const QVariant &property, int role) :
      LayerEdit(),
      _newProperty(property),
      _role(role)
{
	switch (role)
	{
		case RoleOpacity:
		case RoleVisible:
		case RoleBlendMode:
			setModifiedKeys(layer->tileKeysRecursive());
			break;
		default:
			break;
	}
}

void LayerPropertyEdit::redo(Layer *layer)
{
	change(layer);
}

void LayerPropertyEdit::undo(Layer *layer)
{
	change(layer);
}

void LayerPropertyEdit::change(Layer *layer)
{
	auto property = layer->property(_role);
	layer->setProperty(_newProperty, _role);
	_newProperty = property;
}

LayerSurfaceEdit::LayerSurfaceEdit(const Surface &surface, const QPointSet &tileKeys) :
	LayerEdit(),
	_surface(surface)
{
	setModifiedKeys(tileKeys);
}

void LayerSurfaceEdit::redo(Layer *layer)
{
	auto rasterLayer = dynamic_cast<RasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	
	Surface surface = rasterLayer->surface();
	rasterLayer->setSurface(_surface);
	_surface = surface;
}

void LayerSurfaceEdit::undo(Layer *layer)
{
	auto rasterLayer = dynamic_cast<RasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	
	Surface surface = rasterLayer->surface();
	rasterLayer->setSurface(_surface);
	_surface = surface;
}

}
