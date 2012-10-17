#include "core/tabletevent.h"
#include "core/layer.h"
#include "layermovetool.h"
#include "core/widgets/simplebutton.h"
#include "Malachite/mldivision.h"

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
	Painter painter(&surface);
	painter.drawTransformedSurface(_offset, rasterLayer->surface());
	painter.end();
	
	rasterLayer->setSurface(surface);
}

void FSLayerMoveEdit::undo(Layer *layer)
{
	RasterLayer *rasterLayer = dynamic_cast<RasterLayer *>(layer);
	Q_ASSERT(rasterLayer);
	Surface surface;
	Painter painter(&surface);
	painter.drawTransformedSurface(-_offset, rasterLayer->surface());
	rasterLayer->setSurface(surface);
}

LayerMoveTool::LayerMoveTool(CanvasView *parent) :
	Tool(parent)
{}

void LayerMoveTool::drawLayer(SurfacePainter *painter, const Layer *layer)
{
	painter->drawSurface(_offset, layer->surface());
}

void LayerMoveTool::cursorMoveEvent(TabletEvent *event)
{
	if (!_layerIsDragged) return;
	
	_offset = event->data.pos.toQPoint() - _dragStartPoint;
	
	QPointSet keys;
	
	foreach (const QPoint &key, _layer->surface().keys())
	{
		keys |= Surface::keysForRect(Surface::keyToRect(key).translated(_offset));
	}
	
	requestUpdate(keys | _lastKeys);
	_lastKeys = keys;
}

void LayerMoveTool::cursorPressEvent(TabletEvent *event)
{
	_layer = currentLayer();
	if (_layer && _layer->type() == Layer::TypeRaster)
	{
		_layerIsDragged = true;
		addCustomDrawLayer(_layer);
		_dragStartPoint = event->data.pos.toQPoint();
		_lastKeys = _layer->surface().keys();
	}
}

void LayerMoveTool::cursorReleaseEvent(TabletEvent *event)
{
	if (_layerIsDragged)
	{
		_offset = event->data.pos.toQPoint() - _dragStartPoint;
		_layerIsDragged = false;
		clearCustomDrawLayer();
		document()->layerModel()->editLayer(currentLayerIndex(), new FSLayerMoveEdit(_offset), tr("Layer Move"));
	}
}

LayerMoveToolFactory::LayerMoveToolFactory(QObject *parent) :
	ToolFactory(parent)
{
	setObjectName("paintfield.tool.layerMove");
	setText(tr("Move Layer"));
	setIcon(SimpleButton::createSimpleIconSet(":/icons/32x32/move.svg"));
}

bool LayerMoveToolFactory::isTypeSupported(Layer::Type type) const
{
	switch (type)
	{
	case Layer::TypeRaster:
		return true;
	default:
		return false;
	}
}

}

