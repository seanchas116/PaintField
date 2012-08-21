#include "fslayerrenderer.h"

void FSLayerRenderer::renderRecursive(MLPainter *painter, const FSLayer *parent, const QPoint &tileKey, double opacity, bool inDelegateTarget)
{
	QListIterator<const FSLayer *> iter(parent->children());
	iter.toBack();
	
	while (iter.hasPrevious())
	{
		const FSLayer *layer = iter.previous();
		
		if (!layer->visible())
			continue;
		
		bool useDelegate = (_delegate &&  _delegate->target() == layer) || inDelegateTarget;
		
		if (layer->type() == FSLayer::TypeGroup && layer->blendMode().index() == ML::BlendModePassThrough)
		{
			renderRecursive(painter, layer, tileKey, opacity * layer->opacity(), useDelegate);
		}
		else
		{
			painter->setOpacity(opacity * layer->opacity());
			
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
					renderRecursive(&childPainter, layer, tileKey, 1, useDelegate);
				}
				else
				{
					src = layer->surface().tileForKey(tileKey);
				}
				
				painter->drawImage(_pos, src);
			}
		}
	}
}
