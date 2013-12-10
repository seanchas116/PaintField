#include <Malachite/Division>

#include "paintfield/core/layerscene.h"
#include "paintfield/core/canvascursorevent.h"
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

void LayerMoveTool::drawLayer(SurfacePainter *painter, const LayerConstRef &layer)
{
	PAINTFIELD_DEBUG << "offset:" << _offset;
	auto rasterLayer = dynamicSPCast<const RasterLayer>(layer);
	painter->drawSurface(_offset, rasterLayer->surface());
}

void LayerMoveTool::cursorMoveEvent(CanvasCursorEvent *event, int id)
{
	Q_UNUSED(id);
	if (!_layerIsDragged) return;
	
	_offset = event->data.pos.toQPoint() - _dragStartPoint;
	
	auto keys = Surface::offsetKeys(_layer->tileKeys(), _offset);
	requestUpdate(keys | _lastKeys);
	_lastKeys = keys;
}

int LayerMoveTool::cursorPressEvent(CanvasCursorEvent *event)
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
	return 0;
}

void LayerMoveTool::cursorReleaseEvent(CanvasCursorEvent *event, int id)
{
	Q_UNUSED(id);
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

