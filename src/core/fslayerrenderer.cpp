#include "fslayerrenderer.h"

MLImage FSLayerRenderer::renderTile(const FSLayer *root, const QPoint &tileKey, const MLImage &background)
{
	_tileKey = tileKey;
	
	Context context;
	
	renderRecursive(&context, root);
	
	if (context.isEmpty() && background.size() == QSize(MLSurface::TileSize, MLSurface::TileSize))
		return background;
	
	if (background.isValid())
	{
		MLPainter painter(context.destImage());
		
		painter.setBlendMode(ML::BlendModeDestinationOver);
		painter.drawImage(0, 0, background);
	}
	
	return *context.destImage();
}

void FSLayerRenderer::renderRecursive(Context *context, const FSLayer *layer)
{
	if (layer->isVisible() == false)
		return;
	
	if (layer->type() == FSLayer::TypeGroup && layer->blendMode() == ML::BlendModePassThrough)
	{
		//qDebug() << "layer is pass-through group. rendering children...";
		
		double opacity = context->opacity();
		context->setOpacity(opacity * layer->opacity());
		renderChildren(context, layer->children());
		context->setOpacity(opacity);
	}
	else
	{
		MLImage src;
		
		double opacity = layer->opacity() * context->opacity();
		
		if (layer->type() == FSLayer::TypeGroup)
		{
			//qDebug() << "layer is non pass-through group. rendering children to temporary image...";
			
			Context context;
			renderChildren(&context, layer->children());
			src = *context.constDestImage();
		}
		else
		{
			if (_delegate && _delegate->targetLayers().contains(layer))
			{
				//qDebug() << "layer is delegation target.";
				_delegate->render(context, layer, _tileKey);
				return;
			}
			
			MLBlendOp::TileCombination combination = MLBlendOp::NoTile;
			
			if (layer->surface().contains(_tileKey))
				combination |= MLBlendOp::TileSource;
			if (context->isEmpty() == false)
				combination |= MLBlendOp::TileDestination;
			
			switch (layer->blendMode().op()->tileRequirement(combination))
			{
				case MLBlendOp::TileSource:
					//qDebug() << "source only";
					*context->destImage() = layer->surface().tileForKey(_tileKey) * opacity;
				default:
				case MLBlendOp::NoTile:
				case MLBlendOp::TileDestination:
					//qDebug() << "destination only. do nothing.";
					return;
					
				case MLBlendOp::TileBoth:
					break;
			}
			
			//qDebug() << "normal operation";
			src = layer->surface().tileForKey(_tileKey);
		}
		
		MLPainter painter(context->destImage());
		painter.setOpacity(opacity);
		painter.setBlendMode(layer->blendMode());
		painter.drawImage(0, 0, src);
	}
}

void FSLayerRenderer::renderChildren(Context *context, const FSLayerConstList &layers)
{
	QListIterator<const FSLayer *> iter(layers);
	iter.toBack();
	
	while (iter.hasPrevious())
		renderRecursive(context, iter.previous());
}


void FSLayerRenderDelegate::render(FSLayerRenderer::Context *context, const FSLayer *layer, const QPoint &tileKey)
{
	double opacity = layer->opacity() * context->opacity();
	MLBlendOp::TileCombination combination = MLBlendOp::TileSource;
	
	if (context->isEmpty() == false)
		combination |= MLBlendOp::TileDestination;
	
	switch (layer->blendMode().op()->tileRequirement(combination))
	{
		case MLBlendOp::TileSource:
			
			if (_replacingEnabled)
			{
				//qDebug() << "calling renderReplacing";
				*context->destImage() = renderReplacing(layer, tileKey) * opacity;
				return;
			}
			
			break;
			
		case MLBlendOp::TileBoth:
			break;
			
		default:
			return;
	}
	
	//qDebug() << "calling render";
	MLPainter painter(context->destImage());
	painter.setOpacity(opacity);
	painter.setBlendMode(layer->blendMode());
	render(&painter, layer, tileKey);
}

void FSLayerRenderDelegate::render(MLPainter *painter, const FSLayer *layer, const QPoint &tileKey)
{
	Q_UNUSED(painter);
	Q_UNUSED(layer);
	Q_UNUSED(tileKey);
}

MLImage FSLayerRenderDelegate::renderReplacing(const FSLayer *layer, const QPoint &tileKey)
{
	Q_UNUSED(layer);
	Q_UNUSED(tileKey);
	return MLSurface::DefaultTile;
}



