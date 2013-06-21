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
	
	auto keys = Surface::offsetKeys(_layer->tileKeys(), _offset);
	requestUpdate(keys | _lastKeys);
	_lastKeys = keys;
}

void LayerMoveTool::tabletPressEvent(CanvasTabletEvent *event)
{
	PAINTFIELD_DEBUG << "pressed";
	_layer = currentLayer();
	if (_layer && _layer->isType<RasterLayer>() && !_layer->isLocked())
	{
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
		canvas()->document()->layerScene()->editLayer(_layer, new LayerMoveEdit(_offset), tr("Layer Move"));
	}
}

}

