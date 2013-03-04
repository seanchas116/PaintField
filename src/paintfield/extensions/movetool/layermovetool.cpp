#include <Malachite/Division>

#include "paintfield/core/tabletevent.h"
#include "paintfield/core/layer.h"
#include "paintfield/core/widgets/simplebutton.h"

#include "layermovetool.h"

using namespace Malachite;

namespace PaintField
{

class FSLayerMoveEdit : public LayerEdit
{
public:
	explicit FSLayerMoveEdit(const QPoint &offset)
	    : LayerEdit(),
		  _offset(offset)
	{}
	
	void redo(Layer *layer);
	void undo(Layer *layer);
	
private:
	QPoint _offset;
};

void FSLayerMoveEdit::redo(Layer *layer)
{
	RasterLayer *rasterLayer = dynamic_cast<RasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	Surface surface;
	
	{
		Painter painter(&surface);
		painter.drawPreTransformedSurface(_offset, rasterLayer->surface());
	}
	
	surface.squeeze();
	
	rasterLayer->setSurface(surface);
}

void FSLayerMoveEdit::undo(Layer *layer)
{
	RasterLayer *rasterLayer = dynamic_cast<RasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	Surface surface;
	
	{
		Painter painter(&surface);
		painter.drawPreTransformedSurface(-_offset, rasterLayer->surface());
	}
	
	surface.squeeze();
	
	rasterLayer->setSurface(surface);
}

LayerMoveTool::LayerMoveTool(Canvas *parent) :
	Tool(parent)
{}

void LayerMoveTool::drawLayer(SurfacePainter *painter, const Layer *layer)
{
	PAINTFIELD_DEBUG << "offset:" << _offset;
	painter->drawSurface(_offset, layer->surface());
}

void LayerMoveTool::tabletMoveEvent(CanvasTabletEvent *event)
{
	if (!_layerIsDragged) return;
	
	_offset = event->data.pos.toQPoint() - _dragStartPoint;
	
	QPointSet keys;
	
	for (const QPoint &key : _layer->surface().keys())
	{
		keys |= Surface::rectToKeys(Surface::keyToRect(key).translated(_offset));
	}
	
	requestUpdate(keys | _lastKeys);
	_lastKeys = keys;
}

void LayerMoveTool::tabletPressEvent(CanvasTabletEvent *event)
{
	PAINTFIELD_DEBUG << "pressed";
	_layer = currentLayer();
	if (_layer && _layer->isType<RasterLayer>())
	{
		layerModel()->startEditing();
		_layerIsDragged = true;
		addLayerDelegation(_layer);
		_dragStartPoint = event->data.pos.toQPoint();
		_lastKeys = _layer->surface().keys();
	}
}

void LayerMoveTool::tabletReleaseEvent(CanvasTabletEvent *event)
{
	PAINTFIELD_DEBUG << "released";
	if (_layerIsDragged)
	{
		_offset = event->data.pos.toQPoint() - _dragStartPoint;
		_layerIsDragged = false;
		clearLayerDelegation();
		layerModel()->editLayer(currentLayerIndex(), new FSLayerMoveEdit(_offset), tr("Layer Move"));
	}
}

}

