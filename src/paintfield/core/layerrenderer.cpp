#include <Malachite/Container>
#include "layerrenderer.h"

#include <boost/range/adaptor/reversed.hpp>

using namespace Malachite;
using namespace boost;

namespace PaintField
{

Surface LayerRenderer::renderToSurface(const LayerConstPtr &rootLayer, const QPointSet &keyClip, const QHash<QPoint, QRect> &keyRectClip)
{
	Surface surface;
	SurfacePainter painter(&surface);
	
	if (!keyRectClip.isEmpty())
		painter.setKeyRectClip(keyRectClip);
	else
		painter.setKeyClip(keyClip);
	
	renderChildren(&painter, rootLayer);
	
	painter.flush();
	
	return surface;
}

void LayerRenderer::renderLayer(SurfacePainter *painter, const LayerConstPtr &layer)
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

void LayerRenderer::drawLayer(SurfacePainter *painter, const LayerConstPtr &layer)
{
	if (layer->count())
		painter->drawPreTransformedSurface(QPoint(), renderToSurface(layer, painter->keyClip()));
	
	layer->render(painter);
}

void LayerRenderer::renderChildren(SurfacePainter *painter, const LayerConstPtr &parent)
{
	renderLayers(painter, parent->children());
}

void LayerRenderer::renderLayers(SurfacePainter *painter, const QList<LayerConstPtr> &layers)
{
	for (auto layer : layers | adaptors::reversed)
		renderLayer(painter, layer);
}

}

