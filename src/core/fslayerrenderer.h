#ifndef FSLAYERRENDERER_H
#define FSLAYERRENDERER_H

#include "fslayer.h"
#include "mlpainter.h"

class FSLayerRenderDelegate;

class FSLayerRenderer
{
public:
	
	struct Context
	{
		Context(double opacity = 1.0) :
		    _dest(MLSurface::DefaultTile),
		    _opacity(opacity),
		    _isEmpty(true)
		{}
		
		MLImage *destImage()
		{
			_isEmpty = false;
			return &_dest;
		}
		
		const MLImage *constDestImage() const
		{
			return &_dest;
		}
		
		double opacity() const { return _opacity; }
		void setOpacity(double opacity) { _opacity = opacity; }
		
		bool isEmpty() const { return _isEmpty; }
		
		MLImage _dest;
		double _opacity;
		bool _isEmpty;
	};
	
	FSLayerRenderer() : _delegate(0) {}
	
	MLImage renderTile(const FSLayer *root, const QPoint &tileKey, const MLImage &background);
	
	void renderMultiple(MLPainter *painter, const FSLayer *root, const QPointSet &tileKeys)
	{
		foreach (const QPoint &tileKey, tileKeys)
		{
			MLImage tile = renderTile(root, tileKey, MLSurface::WhiteTile);
			painter->drawImage(tileKey * MLSurface::TileSize, tile);
		}
	}
	
	void setDelegate(FSLayerRenderDelegate *delegate) { _delegate = delegate; }
	FSLayerRenderDelegate *delegate() { return _delegate; }
	
protected:
	
private:
	
	void renderRecursive(Context *context, const FSLayer *layer);
	void renderChildren(Context *context, const FSLayerConstList &layers);
	
	FSLayerRenderDelegate *_delegate;
	QPoint _tileKey;
};

class FSLayerRenderDelegate
{
public:
	
	FSLayerRenderDelegate() :
	    _replacingEnabled(false)
	{}
	
	virtual ~FSLayerRenderDelegate() {}
	
	virtual void render(FSLayerRenderer::Context *context, const FSLayer *layer, const QPoint &tileKey);
	virtual void render(MLPainter *painter, const FSLayer *layer, const QPoint &tileKey);
	virtual MLImage renderReplacing(const FSLayer *layer, const QPoint &tileKey);
	
	void addTarget(const FSLayer *layer) { _targetLayers << layer; }
	void addTargets(const FSLayerConstList &layers) { _targetLayers << layers; }
	void clearTargets() { _targetLayers.clear(); }
	FSLayerConstList targetLayers() { return _targetLayers; }
	
	void setReplacingEnabled(bool enabled) { _replacingEnabled = enabled; }
	bool replacingEnabled() const { return _replacingEnabled; }
	
private:
	
	FSLayerConstList _targetLayers;
	bool _replacingEnabled;
};

#endif // FSLAYERRENDERER_H
