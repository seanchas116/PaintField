#include <QtGui>
#include <Malachite/SurfacePainter>

#include "paintfield-core/layeredit.h"
#include "paintfield-core/canvascontroller.h"
#include "paintfield-core/workspacecontroller.h"
#include "paintfield-core/palettemanager.h"
#include "paintfield-core/tabletevent.h"
#include "paintfield-core/debug.h"
#include "paintfield-core/widgets/simplebutton.h"

#include "brushstrokingthread.h"
#include "brushstroker.h"

#include "brushtool.h"


using namespace Malachite;

namespace PaintField {

BrushTool::BrushTool(CanvasView *parent) :
	Tool(parent),
    _thread(new BrushStrokingThread(this))
{
	setCustomCursorEnabled(true);
}

BrushTool::~BrushTool() {}

void BrushTool::drawLayer(SurfacePainter *painter, const Layer *layer)
{
	Q_UNUSED(layer)
	if (isStroking())
	{
		BrushStroker::SurfaceContext context(_thread->stroker());
		painter->drawTransformedSurface(QPoint(), *context.pointer());
	}
}

void BrushTool::drawCustomCursor(QPainter *painter, const Vec2D &pos)
{
	double radius = _brushSize * 0.5 * canvasView()->scale();
	
	QColor color(255, 255, 255, 128);
	
	painter->setCompositionMode(QPainter::CompositionMode_Difference);
	painter->setPen(color);
	painter->setBrush(Qt::NoBrush);
	painter->drawEllipse(pos, radius, radius);
	painter->setPen(Qt::NoPen);
	painter->setBrush(color);
	painter->drawEllipse(pos, 1, 1);
}

QRect BrushTool::customCursorRect(const Vec2D &pos)
{
	double radius = _brushSize * 0.5 * canvasView()->scale() + 0.5;
	return QRectF(pos.x - radius, pos.y - radius, radius * 2, radius * 2).toAlignedRect();
}

void BrushTool::tabletPressEvent(CanvasTabletEvent *event)
{
	event->accept();
}

void BrushTool::tabletMoveEvent(CanvasTabletEvent *event)
{
	if (event->data.pressure())
	{
		if (isStroking())
			drawStroke(event->data);
		else
			beginStroke(event->data);
	}
	else
	{
		if (isStroking())
			endStroke(event->data);
	}
	
	setPrevData(event->data);
	
	event->accept();
}

void BrushTool::tabletReleaseEvent(CanvasTabletEvent *event)
{
	event->accept();
}

void BrushTool::beginStroke(const TabletInput &data)
{
	if (!_strokerFactory)
		return;
	
	_layer = currentLayer();
	
	if (_layer->type() != Layer::TypeRaster)
		return;
	
	PAINTFIELD_CALC_SCOPE_ELAPSED_TIME;
	
	_thread->reset(_strokerFactory->createStroker());
	
	_thread->stroker()->setSurface(_layer->surface());
	_thread->stroker()->setArgb(_argb);
	_thread->stroker()->setRadiusBase(_brushSize * 0.5);
	
	_thread->start();
	
	addCustomDrawLayer(_layer);
	
	// discard pressure for the 1st time to reduce overshoot
	TabletInput newData = data;
	newData.setPressure(0);
	
	if (_dataPrevSet)
	{
		_thread->moveTo(_dataPrev);
		_thread->lineTo(newData);
	}
	else
	{
		_thread->moveTo(newData);
	}
}

void BrushTool::drawStroke(const TabletInput &data)
{
	PAINTFIELD_CALC_SCOPE_ELAPSED_TIME;
	
	_thread->lineTo(data);
	updateTiles();
}

void BrushTool::endStroke(const TabletInput &data)
{
	PAINTFIELD_CALC_SCOPE_ELAPSED_TIME;
	
	_thread->lineTo(data);
	_thread->waitForFinish();
	updateTiles();
	
	if (_layer && _layer == currentLayer())
	{
		document()->layerModel()->makeSkipNextUpdate();
		document()->layerModel()->editLayer(document()->layerModel()->indexForLayer(_layer), new LayerSurfaceEdit(_thread->stroker()->surface(), _thread->stroker()->totalEditedKeys()), tr("Brush"));
	}
	
	_thread->reset();
	clearCustomDrawLayer();
}

void BrushTool::updateTiles()
{
	emit requestUpdate(_thread->stroker()->getAndClearEditedKeysWithRects());
}

void BrushTool::setPrevData(const TabletInput &data)
{
	_dataPrev = data;
	_dataPrevSet = true;
}

void BrushTool::setBrushSize(int size)
{
	_brushSize = size;
}

void BrushTool::setBrushSettings(const QVariantMap &settings)
{
	_settings = settings;
}

bool BrushTool::isStroking() const
{
	return _thread->isStroking();
}

}
