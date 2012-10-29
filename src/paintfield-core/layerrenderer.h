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
	
	Malachite::Surface renderToSurface(const LayerConstList &layers, const QPointSet &keyClip = QPointSet());
	
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
