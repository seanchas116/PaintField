#include "mlsurfacepainter.h"
#include "fsrasterlayer.h"
#include "fslayer.h"
#include "fslayeredit.h"

void FSLayerEdit::saveUndoState(const FSLayer */*layer*/)
{}

void FSLayerEdit::redo(FSLayer */*layer*/)
{}

void FSLayerEdit::undo(FSLayer */*layer*/)
{}

FSLayerPropertyEdit::FSLayerPropertyEdit(const QVariant &property, int role) :
      FSLayerEdit(),
      _newProperty(property),
      _role(role)
{}

void FSLayerPropertyEdit::saveUndoState(const FSLayer *layer)
{
	_oldProperty = layer->property(_role);
}

void FSLayerPropertyEdit::redo(FSLayer *layer)
{
	layer->setProperty(_newProperty, _role);
}

void FSLayerPropertyEdit::undo(FSLayer *layer)
{
	layer->setProperty(_oldProperty, _role);
}

FSLayerSurfaceEdit::FSLayerSurfaceEdit(const MLSurface &surface, const QPointSet &tileKeys) :
	FSLayerEdit(),
	_surface(surface)
{
	setModifiedKeys(tileKeys);
}

void FSLayerSurfaceEdit::saveUndoState(const FSLayer *layer)
{
	if (layer->type() != FSLayer::TypeRaster) {
		qFatal("FSLayerSurfaceEdit::saveUndoState: layer is not raster");
	}
	_oldSurface = layer->surface().section(modifiedKeys());
}

void FSLayerSurfaceEdit::redo(FSLayer *layer)
{
	FSRasterLayer *rasterLayer = dynamic_cast<FSRasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	MLSurface surface = rasterLayer->surface();
	
	MLSurfaceEditor editor(&surface);
	editor.replace(_surface, modifiedKeys());
	rasterLayer->setSurface(surface);
}

void FSLayerSurfaceEdit::undo(FSLayer *layer)
{
	FSRasterLayer *rasterLayer = dynamic_cast<FSRasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	MLSurface surface = rasterLayer->surface();
	
	MLSurfaceEditor editor(&surface);
	editor.replace(_oldSurface, modifiedKeys());
	rasterLayer->setSurface(surface);
}

