#include <QtGui>

#include "fslayeredit.h"
#include "mlsurfacepainter.h"
#include "fstabletevent.h"
#include "fsbrushtool.h"
#include "fsbrushstroker.h"
#include "fsscopedtimer.h"


FSBrushTool::FSBrushTool(FSCanvasView *parent) :
	FSTool(parent),
	_layer(0)
{}

FSBrushTool::~FSBrushTool() {}

void FSBrushTool::render(MLPainter *painter, const FSLayer *layer, const QPoint &tileKey)
{
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
	
#ifdef DEBUG
	qDebug() << "brush stroke start";
#endif
	
	_surface = _layer->surface();
	_stroker.reset(new FSBrushStroker(&_surface));
	_stroker->moveTo(data);
	_inputCount = 0;
	
	setDelegatesRender(true);
}

void FSBrushTool::drawStroke(const FSTabletInputData &data)
{
	if (!_layer)
		return;
	
#ifdef DEBUG
	qDebug() << "brush stroke to" << data.pos;
#endif
	
	_inputCount++;
	_stroker->lineTo(data);
	
	//if (_inputCount % 2 == 0)
		canvas()->updateView(_stroker->lastEditedKeys());
}

void FSBrushTool::endStroke()
{
	if (!_layer)
		return;
	
	setDelegatesRender(false);
	
#ifdef DEBUG
	qDebug() << "brush stroke end";
#endif
	
	FSLayerSurfaceEdit *edit = new FSLayerSurfaceEdit(_surface, _stroker->totalEditedKeys());
	
	documentModel()->editLayer(documentModel()->indexForLayer(_layer), edit);
	
	_stroker.reset();
	_layer = 0;
}

FSBrushToolFactory::FSBrushToolFactory(QObject *parent) :
	FSToolFactory(parent)
{
	setToolName("brush");
	setText(tr("Brush"));
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
