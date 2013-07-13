#ifndef FSLAYERRENDERER_H
#define FSLAYERRENDERER_H

#include "layer.h"
#include <Malachite/SurfacePainter>

namespace PaintField {

class RasterLayer;
class GroupLayer;

struct SurfaceFragment
{
	QPoint tileKey;
	QRect rect;
};

struct SurfaceFragmentImage
{
	QPoint tileKey;
	QPoint pos;
	Malachite::Image image;
};

/*
class LayerFragmentLayer
{
public:
	
	LayerFragmentLayer() {}
	virtual ~LayerFragmentLayer() {}
	
	SurfaceFragmentImage render(const Layer *root, const SurfaceFragment &fragment);
	
protected:
	
	void renderLayer(Malachite::Painter *painter, const QPoint &tileKey, const Layer *layer);
	virtual void drawLayer(Malachite::Painter *painter, const QPoint &tileKey, const Layer *layer);
	virtual void renderChildren(Malachite::Painter *painter, const QPoint &tileKey, const Layer *layer);
	void renderLayers(Malachite::Painter *painter, const QPoint &tileKey, const LayerConstList &layer);
};
*/

class LayerRenderer
{
public:
	
	LayerRenderer() {}
	virtual ~LayerRenderer() {}
	
	/**
	 * Renders layers to a surface.
	 * @param rootLayer The root layer 
	 * @param keyClip Tiles to render (Priority 2)
	 * @param keyRectClip Tiles and their regions to render (Priority 1)
	 * @return
	 */
	Malachite::Surface renderToSurface(const QList<LayerConstRef> &layers, const QPointSet &keyClip, const QHash<QPoint, QRect> &keyRectClip);
	
	Malachite::Surface renderToSurface(const QList<LayerConstRef> &layers, const QPointSet &keyClip = QPointSet())
	{
		return renderToSurface(layers, keyClip, QHash<QPoint, QRect>());
	}
	
protected:
	
	/**
	  Applies "layer"'s opacity and blend mode, and call drawLayer.
	*/
	void renderLayer(Malachite::SurfacePainter *painter, const LayerConstRef &layer);
	
	/**
	  Draws "layer" on "painter". Reimplement this function to customize layer drawing.
	  "painter" is not transformed; you shoud use Malachite::Painter::drawTransformed*.
	  Opacity and blend mode are already applied on painter.
	*/
	virtual void drawLayer(Malachite::SurfacePainter *painter, const LayerConstRef &layer);
	
	virtual void renderChildren(Malachite::SurfacePainter *painter, const LayerConstRef &parent);
	
	void renderLayers(Malachite::SurfacePainter *painter, const QList<LayerConstRef> &layers);
	
private:
	
	
};

}

#endif // FSLAYERRENDERER_H
