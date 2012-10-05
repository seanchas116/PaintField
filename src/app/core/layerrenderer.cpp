
#include "layerrenderer.h"

namespace PaintField
{

using namespace Malachite;

Surface LayerRenderer::render(const LayerConstList &layers, const QPointSet &keyClip)
{
	Surface surface;
	SurfacePainter painter(&surface);
	
	painter.setKeyClip(keyClip);
	
	foreach (const Layer *layer, layers)
		renderLayer(&painter, layer);
	
	return surface;
}

void LayerRenderer::renderLayer(SurfacePainter *painter, const Layer *layer)
{
	double opacity = painter->opacity();
	painter->setOpacity(painter->opacity() * opacity);
	
	if (layer->blendMode() != BlendModePassThrough)
		painter->setBlendMode(layer->blendMode());
	
	drawLayer(painter, layer);
	
	painter->setOpacity(opacity);
}

void LayerRenderer::drawLayer(SurfacePainter *painter, const Layer *layer)
{
	switch (layer->type())
	{
		case Layer::TypeRaster:
			drawRaster(painter, layer);
			break;
			
		case Layer::TypeGroup:
			if (layer->blendMode() == BlendModePassThrough)
				drawGroupPassThrough(painter, layer);
			else
				drawGroup(painter, layer);
			break;
			
		default:
			break;
	}
}

void LayerRenderer::drawRaster(SurfacePainter *painter, const Layer *layer)
{
	painter->drawTransformedSurface(QPoint(), layer->surface());
}

void LayerRenderer::drawGroup(SurfacePainter *painter, const Layer *group)
{
	Surface surface;
	SurfacePainter newPainter(&surface);
	
	newPainter.setShapeTransform(painter->shapeTransform());
	newPainter.setKeyClip(painter->keyClip());
	
	foreach (const Layer *layer, group->children())
	{
		renderLayer(&newPainter, layer);
	}
	
	newPainter.flush();
	
	painter->drawTransformedSurface(QPoint(), surface);
}

void LayerRenderer::drawGroupPassThrough(SurfacePainter *painter, const Layer *group)
{
	foreach (const Layer *layer, group->children())
	{
		renderLayer(painter, layer);
	}
}

}

