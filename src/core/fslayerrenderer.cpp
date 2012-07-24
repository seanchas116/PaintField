#include "fslayerrenderer.h"

void FSLayerRenderer::renderRecursive(MLPainter *painter, const FSLayer *parent, const QPoint &tileKey, bool inDelegateTarget)
{
	QListIterator<const FSLayer *> iter(parent->children());
	iter.toBack();
	
	while (iter.hasPrevious())
	{
		const FSLayer *layer = iter.previous();
		
		if (!layer->visible())
			continue;
		
		bool useDelegate = (_delegate &&  _delegate->target() == layer) || inDelegateTarget;
		painter->setOpacity(painter->opacity() * layer->opacity());
		
		if (layer->type() == FSLayer::TypeGroup && layer->blendMode().index() == MLGlobal::BlendModePassThrough)
		{
			renderRecursive(painter, layer, tileKey, useDelegate);
		}
		else
		{
			painter->setBlendMode(layer->blendMode());
			
			if (layer->type() == FSLayer::TypeRaster && useDelegate)
			{
				_delegate->render(painter, layer, tileKey);
			}
			else
			{
				MLImage src;
				
				if (layer->type() == FSLayer::TypeGroup)
				{
					src = MLSurface::DefaultTile;
					MLPainter childPainter(&src);
					renderRecursive(&childPainter, layer, tileKey, useDelegate);
				}
				else
				{
					src = layer->surface().tileForKey(tileKey);
				}
				
				painter->drawImage(0, 0, src);
			}
		}
	}
}
