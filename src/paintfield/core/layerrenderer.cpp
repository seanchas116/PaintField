#include <Malachite/Container>
#include "layerrenderer.h"

namespace PaintField
{

using namespace Malachite;

Surface LayerRenderer::renderToSurface(const LayerConstList &layers, const QPointSet &keyClip, const QHash<QPoint, QRect> &keyRectClip)
{
	Surface surface;
	SurfacePainter painter(&surface);
	
	painter.setKeyClip(keyClip);
	painter.setKeyRectClip(keyRectClip);
	
	renderLayers(&painter, layers);
	
	painter.flush();
	
	return surface;
}

void LayerRenderer::renderLayer(SurfacePainter *painter, const Layer *layer)
{
	if (!layer->isVisible() || !layer->opacity())
		return;
	
	double opacity = painter->opacity();
	painter->setOpacity(layer->opacity() * opacity);
	
	if (layer->blendMode() == BlendMode::PassThrough)
	{
		renderLayers(painter, layer->children());
	}
	else
	{
		painter->setBlendMode(layer->blendMode());
		drawLayer(painter, layer);
	}
	
	painter->setOpacity(opacity);
}

void LayerRenderer::drawLayer(SurfacePainter *painter, const Layer *layer)
{
	Surface surface;
	
	if (layer->isType<GroupLayer>())
		surface = renderToSurface(layer->children(), painter->keyClip());
	else
		surface = layer->surface();
	
	painter->drawPreTransformedSurface(QPoint(), surface);
}

void LayerRenderer::renderLayers(SurfacePainter *painter, const LayerConstList &layers)
{
	for (const Layer *layer : reverseContainer(layers))
		renderLayer(painter, layer);
}

}

