#include <Malachite/SurfacePainter>

#include "layer.h"
#include "rasterlayer.h"

#include "layeredit.h"

namespace PaintField
{

using namespace Malachite;

void LayerEdit::redo(const LayerPtr &layer)
{
	Q_UNUSED(layer)
}

void LayerEdit::undo(const LayerPtr &layer)
{
	Q_UNUSED(layer)
}

LayerPropertyEdit::LayerPropertyEdit(const LayerPtr &layer, const QVariant &property, int role) :
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

void LayerPropertyEdit::redo(const LayerPtr &layer)
{
	change(layer);
}

void LayerPropertyEdit::undo(const LayerPtr &layer)
{
	change(layer);
}

void LayerPropertyEdit::change(const LayerPtr &layer)
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

void LayerSurfaceEdit::redo(const LayerPtr &layer)
{
	auto rasterLayer = std::dynamic_pointer_cast<RasterLayer>(layer);
	Q_ASSERT(rasterLayer);
	
	Surface surface = rasterLayer->surface();
	rasterLayer->setSurface(_surface);
	_surface = surface;
}

void LayerSurfaceEdit::undo(const LayerPtr &layer)
{
	auto rasterLayer = std::dynamic_pointer_cast<RasterLayer>(layer);
	Q_ASSERT(rasterLayer);
	
	Surface surface = rasterLayer->surface();
	rasterLayer->setSurface(_surface);
	_surface = surface;
}

}
