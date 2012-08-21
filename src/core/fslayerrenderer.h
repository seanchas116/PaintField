#ifndef FSLAYERRENDERER_H
#define FSLAYERRENDERER_H

#include "fslayer.h"
#include "mlpainter.h"

class FSLayerRenderDelegate
{
public:
	virtual void render(MLPainter *painter, const FSLayer *layer, const QPoint &tileKey) = 0;
	virtual const FSLayer *target() = 0;
};

class FSLayerRenderer
{
public:
	
	FSLayerRenderer() : _delegate(0) {}
	
	void render(MLPainter *painter, const FSLayer *parent, const QPoint &tileKey, const QPoint &pos = QPoint())
	{
		_pos = pos;
		renderRecursive(painter, parent, tileKey, 1, false);
	}

	void renderMultiple(MLPainter *painter, const FSLayer *parent, const QPointSet &tileKeys)
	{
		foreach (const QPoint &tileKey, tileKeys)
		{
			render(painter, parent, tileKey, tileKey * MLSurface::TileSize);
		}
	}
	
	void setDelegate(FSLayerRenderDelegate *delegate) { _delegate = delegate; }
	FSLayerRenderDelegate *delegate() { return _delegate; }
	
private:
	
	void renderRecursive(MLPainter *painter, const FSLayer *parent, const QPoint &tileKey, double opacity, bool inDelegateTarget);
	
	FSLayerRenderDelegate *_delegate;
	QPoint _pos;
};

#endif // FSLAYERRENDERER_H
