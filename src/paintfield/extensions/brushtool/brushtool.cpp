#include <QtGui>
#include <Malachite/SurfacePainter>

#include "paintfield/core/layeredit.h"
#include "paintfield/core/canvas.h"
#include "paintfield/core/workspace.h"
#include "paintfield/core/palettemanager.h"
#include "paintfield/core/tabletevent.h"
#include "paintfield/core/widgets/simplebutton.h"

#include "brushstroker.h"

#include "brushtool.h"


using namespace Malachite;

namespace PaintField {

BrushTool::BrushTool(Canvas *parent) :
	Tool(parent)
{
	//setCustomCursorEnabled(true);
}

BrushTool::~BrushTool() {}

void BrushTool::drawLayer(SurfacePainter *painter, const Layer *layer)
{
	Q_UNUSED(layer)
	painter->drawPreTransformedSurface(QPoint(), _surface);
}

void BrushTool::drawCustomCursor(QPainter *painter, const Vec2D &pos)
{
	double radius = _brushSize * 0.5 * canvas()->scale();
	
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
	double radius = _brushSize * 0.5 * canvas()->scale() + 0.5;
	return QRectF(pos.x() - radius, pos.y() - radius, radius * 2, radius * 2).toAlignedRect();
}

void BrushTool::tabletPressEvent(CanvasTabletEvent *event)
{
	event->accept();
}

void BrushTool::tabletMoveEvent(CanvasTabletEvent *event)
{
	if (event->data.pressure)
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

void BrushTool::beginStroke(const TabletInputData &data)
{
	if (!_strokerFactory)
		return;
	
	_layer = currentLayer();
	
	if (_layer->type() != Layer::TypeRaster)
		return;
	
	layerModel()->startEditing();
	
	//PAINTFIELD_CALC_SCOPE_ELAPSED_TIME;
	
	_surface = _layer->surface();
	
	_stroker.reset(_strokerFactory->createStroker(&_surface));
	_stroker->loadSettings(_settings);
	_stroker->setPixel(_pixel);
	_stroker->setRadiusBase(double(_brushSize) * 0.5);
	
	addCustomDrawLayer(_layer);
	
	// discard pressure for the 1st time to reduce overshoot
	TabletInputData newData = data;
	newData.pressure = 0;
	
	if (_dataPrevSet)
	{
		_stroker->moveTo(_dataPrev);
		_stroker->lineTo(newData);
	}
	else
	{
		_stroker->moveTo(newData);
	}
}

void BrushTool::drawStroke(const TabletInputData &data)
{
	if (!isStroking())
		return;
	
	//PAINTFIELD_CALC_SCOPE_ELAPSED_TIME;
	
	_stroker->lineTo(data);
	updateTiles();
}

void BrushTool::endStroke(const TabletInputData &data)
{
	if (!isStroking())
		return;
	
	//PAINTFIELD_CALC_SCOPE_ELAPSED_TIME;
	
	_stroker->lineTo(data);
	_stroker->end();
	updateTiles();
	
	if (_layer && _layer == currentLayer())
	{
		_surface.squeeze(_stroker->totalEditedKeys());
		canvas()->makeSkipNextUpdate();
		document()->layerModel()->editLayer(document()->layerModel()->indexForLayer(_layer), new LayerSurfaceEdit(_surface, _stroker->totalEditedKeys()), tr("Brush"));
	}
	
	_stroker.reset();
	clearCustomDrawLayer();
}

void BrushTool::updateTiles()
{
	emit requestUpdate(_stroker->lastEditedKeysWithRects());
	_stroker->clearLastEditedKeys();
}

void BrushTool::setPrevData(const TabletInputData &data)
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

}
