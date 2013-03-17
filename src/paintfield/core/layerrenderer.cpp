#include <Malachite/Container>
#include "layerrenderer.h"

namespace PaintField
{

using namespace Malachite;

Surface LayerRenderer::renderToSurface(const Layer *rootLayer, const QPointSet &keyClip, const QHash<QPoint, QRect> &keyRectClip)
{
	Surface surface;
	SurfacePainter painter(&surface);
	
	painter.setKeyClip(keyClip);
	painter.setKeyRectClip(keyRectClip);
	
	renderChildren(&painter, rootLayer);
	
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
		renderChildren(painter, layer);
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
	if (layer->count())
		painter->drawPreTransformedSurface(QPoint(), renderToSurface(layer, painter->keyClip()));
	
	layer->render(painter);
}

void LayerRenderer::renderChildren(SurfacePainter *painter, const Layer *parent)
{
	renderLayers(painter, parent->children());
}

void LayerRenderer::renderLayers(SurfacePainter *painter, const LayerConstList &layers)
{
	QListIterator<const Layer *> iter(layers);
	iter.toBack();
	
	while (iter.hasPrevious())
		renderLayer(painter, iter.previous());
}

}

