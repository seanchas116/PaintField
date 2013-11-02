#include <Malachite/SurfacePainter>

#include "layer.h"
#include "rasterlayer.h"

#include "layeredit.h"

namespace PaintField
{

using namespace Malachite;

void LayerEdit::redo(const LayerRef &layer)
{
	Q_UNUSED(layer)
}

void LayerEdit::undo(const LayerRef &layer)
{
	Q_UNUSED(layer)
}

LayerPropertyEdit::LayerPropertyEdit(const LayerRef &layer, const QVariant &property, int role) :
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

void LayerPropertyEdit::redo(const LayerRef &layer)
{
	change(layer);
}

void LayerPropertyEdit::undo(const LayerRef &layer)
{
	change(layer);
}

void LayerPropertyEdit::change(const LayerRef &layer)
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

void LayerSurfaceEdit::redo(const LayerRef &layer)
{
	auto rasterLayer = dynamicSPCast<RasterLayer>(layer);
	Q_ASSERT(rasterLayer);
	
	Surface surface = rasterLayer->surface();
	rasterLayer->setSurface(_surface);
	_surface = surface;
}

void LayerSurfaceEdit::undo(const LayerRef &layer)
{
	auto rasterLayer = dynamicSPCast<RasterLayer>(layer);
	Q_ASSERT(rasterLayer);
	
	Surface surface = rasterLayer->surface();
	rasterLayer->setSurface(_surface);
	_surface = surface;
}

void LayerMoveEdit::redo(const LayerRef &layer)
{
	auto rasterLayer = dynamicSPCast<RasterLayer>(layer);
	Q_ASSERT(rasterLayer);
	Surface surface;
	
	{
		Painter painter(&surface);
		painter.drawPreTransformedSurface(_offset, rasterLayer->surface());
	}
	
	surface.squeeze();
	
	rasterLayer->setSurface(surface);
}

void LayerMoveEdit::undo(const LayerRef &layer)
{
	auto rasterLayer = dynamicSPCast<RasterLayer>(layer);
	Q_ASSERT(rasterLayer);
	Surface surface;
	
	{
		Painter painter(&surface);
		painter.drawPreTransformedSurface(-_offset, rasterLayer->surface());
	}
	
	surface.squeeze();
	
	rasterLayer->setSurface(surface);
}

}
