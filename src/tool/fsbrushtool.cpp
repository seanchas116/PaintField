#include <QtGui>

#include "fslayeredit.h"
#include "mlsurfacepainter.h"
#include "fstabletevent.h"
#include "fsbrushtool.h"
#include "fsbrushstroker.h"
#include "fsscopedtimer.h"


FSBrushTool::FSBrushTool(FSCanvasView *parent) :
	FSTool(parent),
	_dataIsSet(false),
	_brushSetting(0),
	_layer(0)
{}

FSBrushTool::~FSBrushTool() {}

void FSBrushTool::render(MLPainter *painter, const FSLayer *layer, const QPoint &tileKey)
{
	Q_UNUSED(layer);
	if (_surface.contains(tileKey))
		painter->drawImage(0, 0, _surface.tileForKey(tileKey));
}

void FSBrushTool::cursorPressEvent(FSTabletEvent *event)
{
	beginStroke(event->data);
	event->accept();
}

void FSBrushTool::cursorMoveEvent(FSTabletEvent *event)
{
	drawStroke(event->data);
	event->accept();
}

void FSBrushTool::cursorReleaseEvent(FSTabletEvent *event)
{
	endStroke();
	event->accept();
}

void FSBrushTool::beginStroke(const FSTabletInputData &data)
{
	if (_layer)
		return;
	
	_layer = currentLayer();
	if (_layer->type() != FSLayer::TypeRaster) {
		_layer = 0;
		return;
	}
	
#ifdef QT_DEBUG
	qDebug() << "brush stroke start";
#endif
	
	_surface = _layer->surface();
	_stroker.reset(new FSBrushStroker(&_surface, _brushSetting));
	
	setDelegatesRender(true);
	
	if (_dataIsSet)
	{
		_stroker->moveTo(_data);
		updateTiles();
		drawStroke(data);
	}
	else
	{
		_stroker->moveTo(data);
		updateTiles();
	}
}

void FSBrushTool::drawStroke(const FSTabletInputData &data)
{
	if (!_layer)
	{
		_data = data;
		_dataIsSet = true;
		return;
	}
	
#ifdef QT_DEBUG
	qDebug() << "brush stroke to" << (QPointF)data.pos;
#endif
	
	_stroker->lineTo(data);
	updateTiles();
}

void FSBrushTool::endStroke()
{
	if (!_layer)
		return;
	
	_stroker->end();
	updateTiles();
	
	setDelegatesRender(false);
	
#ifdef QT_DEBUG
	qDebug() << "brush stroke end";
#endif
	
	FSLayerSurfaceEdit *edit = new FSLayerSurfaceEdit(_surface, _stroker->totalEditedKeys());
	
	documentModel()->editLayer(documentModel()->indexForLayer(_layer), edit);
	
	_stroker.reset();
	_layer = 0;
}

void FSBrushTool::updateTiles()
{
	canvas()->updateView(_stroker->lastEditedKeys());
	_stroker->clearLastEditedKeys();
}

FSBrushToolFactory::FSBrushToolFactory(QObject *parent) :
	FSToolFactory(parent)
{
	setToolName("brush");
	setText(tr("Brush"));
}

FSTool *FSBrushToolFactory::createTool(FSCanvasView *view)
{
	FSBrushTool *tool = new FSBrushTool(view);
	tool->setBrushSetting(&_setting);
	return tool;
}

bool FSBrushToolFactory::isTypeSupported(FSLayer::Type type) const
{
	switch (type) {
	case FSLayer::TypeRaster:
		return true;
	default:
		return false;
	}
}
