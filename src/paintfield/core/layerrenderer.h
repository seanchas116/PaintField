#ifndef FSLAYERRENDERER_H
#define FSLAYERRENDERER_H

#include "layer.h"
#include <Malachite/SurfacePainter>

namespace PaintField {

class RasterLayer;
class GroupLayer;


class LayerRenderer
{
public:
	
	LayerRenderer() {}
	virtual ~LayerRenderer() {}
	
	/**
	 * Renders layers to a surface.
	 * @param layers Layers to render
	 * @param keyClip Tiles to render (Priority 2)
	 * @param keyRectClip Tiles and their regions to render (Priority 1)
	 * @return
	 */
	Malachite::Surface renderToSurface(const LayerConstList &layers, const QPointSet &keyClip = QPointSet(), const QHash<QPoint, QRect> &keyRectClip = QHash<QPoint, QRect>());
	
protected:
	
	/**
	  Applies "layer"'s opacity and blend mode, and call drawLayer.
	*/
	void renderLayer(Malachite::SurfacePainter *painter, const Layer *layer);
	
	/**
	  Draws "layer" on "painter". Reimplement this function to customize layer drawing.
	  "painter" is not transformed; you shoud use Malachite::Painter::drawTransformed*.
	  Opacity and blend mode are already applied on painter.
	*/
	virtual void drawLayer(Malachite::SurfacePainter *painter, const Layer *layer);
	
private:
	
	void renderLayers(Malachite::SurfacePainter *painter, const LayerConstList &layers);
};

}

#endif // FSLAYERRENDERER_H
