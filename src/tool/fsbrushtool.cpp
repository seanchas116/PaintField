#include <QtGui>

#include "fslayeredit.h"
#include "mlsurfacepainter.h"
#include "fstabletevent.h"
#include "fsbrushtool.h"
#include "fsbrushstroker.h"
#include "fsscopedtimer.h"


FSBrushTool::FSBrushTool(FSCanvasView *parent) :
	FSTool(parent),
	_dataPrevSet(false),
	_trailing(false),
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
	event->accept();
}

void FSBrushTool::cursorMoveEvent(FSTabletEvent *event)
{
#ifdef QT_DEBUG
	qDebug() << "tablet event x:" << event->data.pos.x << "y:" << event->data.pos.y << "pressure:" << event->data.pressure;
#endif
	
	if (_stroker)
	{
		if (_trailing)
		{
			_trailing = false;
			endStroke(event->data);
		}
		else
		{
			drawStroke(event->data);
			if (event->data.pressure == 0)
				_trailing = true;
		}
	}
	else if (event->data.pressure)
	{
		if (!_dataPrevSet)
			setPrevData(event->data);
		
		beginStroke(event->data);
	}
	
	setPrevData(event->data);
	
	event->accept();
}

void FSBrushTool::cursorReleaseEvent(FSTabletEvent *event)
{
	event->accept();
}

void FSBrushTool::beginStroke(const FSTabletInputData &data)
{
	FSScopedTimer timer(Q_FUNC_INFO);
	
	_layer = currentLayer();
	if (_layer->type() != FSLayer::TypeRaster)
	{
		return;
	}
	
	_surface = _layer->surface();
	_stroker.reset(new FSBrushStroker(&_surface, _brushSetting));
	
	setDelegatesRender(true);
	
	_stroker->moveTo(_dataBeforePrev);
	_stroker->lineTo(_dataPrev);
	_stroker->lineTo(data);
}

void FSBrushTool::drawStroke(const FSTabletInputData &data)
{
	FSScopedTimer timer(Q_FUNC_INFO);
	
	_stroker->lineTo(data);
	updateTiles();
}

void FSBrushTool::endStroke(const FSTabletInputData &data)
{
	FSScopedTimer timer(Q_FUNC_INFO);
	
	_stroker->lineTo(data);
	_stroker->end();
	updateTiles();
	
	setDelegatesRender(false);
	
	//documentModel()->setData(documentModel()->indexForLayer(_layer), QVariant::fromValue(_surface), FSGlobal::RoleSurface, tr("Brush"));
	documentModel()->makeSkipNextUpdate();
	documentModel()->editLayer(documentModel()->indexForLayer(_layer), new FSLayerSurfaceEdit(_surface, _stroker->totalEditedKeys()), tr("Brush"));
	
	_stroker.reset();
}

void FSBrushTool::updateTiles()
{
	canvas()->updateView(_stroker->lastEditedKeys());
	_stroker->clearLastEditedKeys();
}

void FSBrushTool::setPrevData(const FSTabletInputData &data)
{
	if (_dataPrevSet)
	{
		_dataBeforePrev = _dataPrev;
		_dataPrev = data;
	}
	else
	{
		_dataBeforePrev = data;
		_dataPrev = data;
		_dataPrevSet = true;
	}
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
