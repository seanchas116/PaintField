#include <Malachite/Division>

#include "paintfield/core/layerscene.h"
#include "paintfield/core/tabletevent.h"
#include "paintfield/core/layer.h"
#include "paintfield/core/widgets/simplebutton.h"
#include "paintfield/core/rasterlayer.h"

#include "layermovetool.h"

using namespace Malachite;

namespace PaintField
{

class LayerMoveEdit : public LayerEdit
{
public:
	explicit LayerMoveEdit(const QPoint &offset)
	    : LayerEdit(),
		  _offset(offset)
	{}
	
	void redo(const LayerPtr &layer);
	void undo(const LayerPtr &layer);
	
private:
	QPoint _offset;
};

void LayerMoveEdit::redo(const LayerPtr &layer)
{
	auto rasterLayer = std::dynamic_pointer_cast<RasterLayer>(layer);
	Q_ASSERT(rasterLayer);
	Surface surface;
	
	{
		Painter painter(&surface);
		painter.drawPreTransformedSurface(_offset, rasterLayer->surface());
	}
	
	surface.squeeze();
	
	rasterLayer->setSurface(surface);
}

void LayerMoveEdit::undo(const LayerPtr &layer)
{
	auto rasterLayer = std::dynamic_pointer_cast<RasterLayer>(layer);
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

void LayerMoveTool::drawLayer(SurfacePainter *painter, const LayerConstPtr &layer)
{
	PAINTFIELD_DEBUG << "offset:" << _offset;
	auto rasterLayer = std::dynamic_pointer_cast<const RasterLayer>(layer);
	painter->drawSurface(_offset, rasterLayer->surface());
}

void LayerMoveTool::tabletMoveEvent(CanvasTabletEvent *event)
{
	if (!_layerIsDragged) return;
	
	_offset = event->data.pos.toQPoint() - _dragStartPoint;
	
	QPointSet keys;
	
	for (const QPoint &key : _layer->tileKeys())
		keys |= Surface::rectToKeys(Surface::keyToRect(key).translated(_offset));
	
	requestUpdate(keys | _lastKeys);
	_lastKeys = keys;
}

void LayerMoveTool::tabletPressEvent(CanvasTabletEvent *event)
{
	PAINTFIELD_DEBUG << "pressed";
	_layer = currentLayer();
	if (_layer && _layer->isType<RasterLayer>() && !_layer->isLocked())
	{
		canvas()->document()->layerScene()->abortThumbnailUpdate();
		_layerIsDragged = true;
		addLayerDelegation(_layer);
		_dragStartPoint = event->data.pos.toQPoint();
		_lastKeys = _layer->tileKeys();
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
		canvas()->document()->layerScene()->editLayer(currentLayer(), new LayerMoveEdit(_offset), tr("Layer Move"));
	}
}

}

