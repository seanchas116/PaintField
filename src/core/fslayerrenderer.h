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
	
	void render(MLPainter *painter, const FSLayer *parent, const QPoint &tileKey)
	{
		renderRecursive(painter, parent, tileKey, false);
	}

	void render(MLPainter *painter, const FSLayer *parent, const QPointSet &tileKeys)
	{
		foreach (const QPoint &tileKey, tileKeys)
		{
			render(painter, parent, tileKey);
		}
	}
	
	void setDelegate(FSLayerRenderDelegate *delegate) { _delegate = delegate; }
	FSLayerRenderDelegate *delegate() { return _delegate; }
	
private:
	
	void renderRecursive(MLPainter *painter, const FSLayer *parent, const QPoint &tileKey, bool inDelegateTarget);
	
	FSLayerRenderDelegate *_delegate;
};

#endif // FSLAYERRENDERER_H
