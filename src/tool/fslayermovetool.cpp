#include "fstabletevent.h"
#include "fsrasterlayer.h"
#include "fslayermovetool.h"
#include "mlintdivision.h"

void FSLayerMoveEdit::redo(FSLayer *layer)
{
	FSRasterLayer *rasterLayer = dynamic_cast<FSRasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	MLSurface surface;
	MLPainter painter(&surface);
	painter.drawSurface(_offset, rasterLayer->surface());
	painter.end();
	
	rasterLayer->setSurface(surface);
}

void FSLayerMoveEdit::undo(FSLayer *layer)
{
	FSRasterLayer *rasterLayer = dynamic_cast<FSRasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	MLSurface surface;
	MLPainter painter(&surface);
	painter.drawSurface(-_offset, rasterLayer->surface());
	rasterLayer->setSurface(surface);
}

FSLayerMoveTool::FSLayerMoveTool(FSCanvasView *parent) :
	FSTool(parent),
	_layerIsDragged(false)
{}

void FSLayerMoveTool::render(MLPainter *painter, const FSLayer *layer, const QPoint &tileKey)
{
	const MLSurface surface = layer->surface();
	QPoint offsetTile, offsetRemainder;
	MLIntDivision::dividePoint(_offset, MLSurface::TileSize, &offsetTile, &offsetRemainder);
	
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

void FSLayerMoveTool::cursorMoveEvent(FSTabletEvent *event)
{
	if (!_layerIsDragged) return;
	_offset = event->data.pos.toPoint() - _dragStartPoint;
	canvas()->updateView();
}

void FSLayerMoveTool::cursorPressEvent(FSTabletEvent *event)
{
	_layer = currentLayer();
	if (_layer && _layer->type() == FSLayer::TypeRaster)
	{
		_layerIsDragged = true;
		setDelegatesRender(true);
		_dragStartPoint = event->data.pos.toPoint();
	}
}

void FSLayerMoveTool::cursorReleaseEvent(FSTabletEvent *event)
{
	if (_layerIsDragged)
	{
		_layerIsDragged = false;
		setDelegatesRender(false);
		_offset = event->data.pos.toPoint() - _dragStartPoint;
		documentModel()->editLayer(currentLayerIndex(), new FSLayerMoveEdit(_offset));
	}
}

FSLayerMoveToolFactory::FSLayerMoveToolFactory(QObject *parent) :
	FSToolFactory(parent)
{
	setToolName("layerMove");
	setText(tr("Move Layer"));
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

