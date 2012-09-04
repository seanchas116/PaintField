#include "fstabletevent.h"
#include "fsrasterlayer.h"
#include "fslayermovetool.h"
#include "fssimplebutton.h"
#include "mldivision.h"

void FSLayerMoveEdit::redo(FSLayer *layer)
{
	FSRasterLayer *rasterLayer = dynamic_cast<FSRasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	MLSurface surface;
	MLPainter painter(&surface);
	painter.drawTransformedSurface(_offset, rasterLayer->surface());
	painter.end();
	
	rasterLayer->setSurface(surface);
}

void FSLayerMoveEdit::undo(FSLayer *layer)
{
	FSRasterLayer *rasterLayer = dynamic_cast<FSRasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	MLSurface surface;
	MLPainter painter(&surface);
	painter.drawTransformedSurface(-_offset, rasterLayer->surface());
	rasterLayer->setSurface(surface);
}

class FSLayerMoveRenderDelegate : public FSLayerRenderDelegate
{
public:
	
	FSLayerMoveRenderDelegate() : FSLayerRenderDelegate() {}
	
	void setOffset(const QPoint &offset) { _offset = offset; }
	
	void render(MLPainter *painter, const FSLayer *layer, const QPoint &tileKey)
	{
		const MLSurface surface = layer->surface();
		QPoint offsetTile, offsetRemainder;
		mlDividePoint(_offset, MLSurface::TileSize, &offsetTile, &offsetRemainder);
		
		QPoint srcTileKey = tileKey - offsetTile;
		
		if (surface.contains(srcTileKey - QPoint(1 ,1) ) )
			painter->drawImage(offsetRemainder - QPoint(MLSurface::TileSize, MLSurface::TileSize),
			                   surface.tileForKey(srcTileKey - QPoint(1, 1) ) );
		
		if (surface.contains(srcTileKey - QPoint(0 ,1) ) )
			painter->drawImage(offsetRemainder - QPoint(0, MLSurface::TileSize),
			                   surface.tileForKey(srcTileKey - QPoint(0, 1) ) );
		
		if (surface.contains(srcTileKey - QPoint(1 ,0) ) )
			painter->drawImage(offsetRemainder - QPoint(MLSurface::TileSize, 0),
			                   surface.tileForKey(srcTileKey - QPoint(1, 0) ) );
		
		if (surface.contains(srcTileKey) )
			painter->drawImage(offsetRemainder,
			                   surface.tileForKey(srcTileKey) );
	}
	
private:
	
	QPoint _offset;
};

FSLayerMoveTool::FSLayerMoveTool(FSCanvas *parent) :
	FSTool(parent),
	_layerIsDragged(false)
{
	_delegate.reset(new FSLayerMoveRenderDelegate);
}

FSLayerMoveTool::~FSLayerMoveTool()
{}

FSLayerRenderDelegate *FSLayerMoveTool::renderDelegate()
{
	return _delegate.data();
}

void FSLayerMoveTool::cursorMoveEvent(FSTabletEvent *event)
{
	if (!_layerIsDragged) return;
	
	QPoint offset = event->data.pos.toQPoint() - _dragStartPoint;
	
	QPointSet keys;
	
	foreach (const QPoint &key, _layer->surface().keys())
	{
		keys |= MLSurface::keysForRect(MLSurface::keyToRect(key).translated(offset));
	}
	
	_delegate->setOffset(offset);
	
	requestUpdate(keys | _lastKeys);
	_lastKeys = keys;
}

void FSLayerMoveTool::cursorPressEvent(FSTabletEvent *event)
{
	_layer = currentLayer();
	if (_layer && _layer->type() == FSLayer::TypeRaster)
	{
		_layerIsDragged = true;
		_delegate->addTarget(_layer);
		_dragStartPoint = event->data.pos.toQPoint();
		_lastKeys = _layer->surface().keys();
	}
}

void FSLayerMoveTool::cursorReleaseEvent(FSTabletEvent *event)
{
	if (_layerIsDragged)
	{
		QPoint offset = event->data.pos.toQPoint() - _dragStartPoint;
		_layerIsDragged = false;
		_delegate->clearTargets();
		document()->editLayer(currentLayerIndex(), new FSLayerMoveEdit(offset), tr("Layer Move"));
	}
}

FSLayerMoveToolFactory::FSLayerMoveToolFactory(QObject *parent) :
	FSToolFactory(parent)
{
	setToolName("layerMove");
	setText(tr("Move Layer"));
	setIcon(fsCreateSimpleIconSet(":/icons/32x32/move.svg"));
}

bool FSLayerMoveToolFactory::isTypeSupported(FSLayer::Type type) const
{
	switch (type) {
	case FSLayer::TypeRaster:
		return true;
	default:
		return false;
	}
}

