#include <QtGui>

#include "modules/document/layeredit.h"
#include "mlsurfacepainter.h"
#include "core/tabletevent.h"
#include "brushtool.h"
#include "brushstroker.h"
#include "core/scopedtimer.h"
#include "widgets/simplebutton.h"
#include "brushsettingpanel.h"

namespace PaintField
{

BrushTool::BrushTool(Canvas *parent) :
	Tool(parent),
	_dataPrevSet(false),
	_trailing(false),
    _trailingEnabled(false),
	_brushSetting(0),
	_layer(0)
{
}

BrushTool::~BrushTool() {}

void BrushTool::drawLayer(MLSurfacePainter *painter, const Layer *layer)
{
	Q_UNUSED(layer)
	painter->drawTransformedSurface(QPoint(), _surface);
}

void BrushTool::cursorPressEvent(TabletEvent *event)
{
	event->accept();
}

void BrushTool::cursorMoveEvent(TabletEvent *event)
{
#ifdef QT_DEBUG
	qDebug() << "tablet event x:" << event->data.pos.x << "y:" << event->data.pos.y << "pressure:" << event->data.pressure;
#endif
	
	if (_stroker)
	{
		if (_trailingEnabled)
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
		else
		{
			if (event->data.pressure)
				drawStroke(event->data);
			else
				endStroke(event->data);
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

void BrushTool::cursorReleaseEvent(TabletEvent *event)
{
	event->accept();
}

void BrushTool::beginStroke(const TabletInputData &data)
{
	ScopedTimer timer(Q_FUNC_INFO);
	
	_layer = currentLayer();
	if (_layer->type() != Layer::TypeRaster)
	{
		return;
	}
	
	_surface = _layer->surface();
	_stroker.reset(new FSPenStroker(&_surface, _brushSetting));
	addCustomDrawLayer(_layer);
	
	// discard pressure for the 1st time to reduce overshoot
	TabletInputData newData = data;
	newData.pressure = 0;
	
	if (_trailingEnabled)
	{
		_stroker->moveTo(_dataBeforePrev);
		_stroker->lineTo(_dataPrev);
		_stroker->lineTo(newData);
	}
	else
	{
		_stroker->moveTo(_dataPrev);
		_stroker->lineTo(newData);
	}
}

void BrushTool::drawStroke(const TabletInputData &data)
{
	ScopedTimer timer(Q_FUNC_INFO);
	
	_stroker->lineTo(data);
	updateTiles();
}

void BrushTool::endStroke(const TabletInputData &data)
{
	ScopedTimer timer(Q_FUNC_INFO);
	
	_stroker->lineTo(data);
	_stroker->end();
	updateTiles();
	
	//documentModel()->setData(documentModel()->indexForLayer(_layer), QVariant::fromValue(_surface), FSGlobal::RoleSurface, tr("Brush"));
	document()->layerModel()->makeSkipNextUpdate();
	document()->layerModel()->editLayer(document()->layerModel()->indexForLayer(_layer), new FSLayerSurfaceEdit(_surface, _stroker->totalEditedKeys()), tr("Brush"));
	
	_stroker.reset();
	clearCustomDrawLayer();
}

void BrushTool::updateTiles()
{
	emit requestUpdate(_stroker->lastEditedKeys());
	_stroker->clearLastEditedKeys();
}

void BrushTool::setPrevData(const TabletInputData &data)
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

BrushToolFactory::BrushToolFactory(QObject *parent) :
	ToolFactory(parent)
{
	setToolName("brush");
	setText(tr("Brush"));
	setIcon(createSimpleIconSet(":/icons/32x32/brush.svg"));
}

Tool *BrushToolFactory::createTool(Canvas *parent)
{
	BrushTool *tool = new BrushTool(parent);
	tool->setBrushSetting(&_setting);
	return tool;
}

bool BrushToolFactory::isTypeSupported(Layer::Type type) const
{
	switch (type) {
	case Layer::TypeRaster:
		return true;
	default:
		return false;
	}
}

}
