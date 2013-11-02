#include "layerrenderer.h"

#include <amulet/range_extension.hh>
#include <Malachite/Container>

using namespace Malachite;

namespace PaintField
{

Surface LayerRenderer::renderToSurface(const QList<LayerConstRef> &layers, const QPointSet &keyClip, const QHash<QPoint, QRect> &keyRectClip)
{
	Surface surface;
	SurfacePainter painter(&surface);
	
	if (!keyRectClip.isEmpty())
		painter.setKeyRectClip(keyRectClip);
	else
		painter.setKeyClip(keyClip);
	
	renderLayers(&painter, layers);
	
	painter.flush();
	
	return surface;
}

void LayerRenderer::renderLayer(SurfacePainter *painter, const LayerConstRef &layer)
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

void LayerRenderer::drawLayer(SurfacePainter *painter, const LayerConstRef &layer)
{
	if (layer->count())
		painter->drawPreTransformedSurface(QPoint(), renderToSurface(layer->children(), painter->keyClip()));
	
	layer->render(painter);
}

void LayerRenderer::renderChildren(SurfacePainter *painter, const LayerConstRef &parent)
{
	renderLayers(painter, parent->children());
}

void LayerRenderer::renderLayers(SurfacePainter *painter, const QList<LayerConstRef> &layers)
{
	for (auto layer : layers++.reverse())
		renderLayer(painter, layer);
}

}

