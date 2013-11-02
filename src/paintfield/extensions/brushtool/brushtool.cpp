#include <QApplication>
#include <QTimer>
#include <Malachite/SurfacePainter>

#include "paintfield/core/layerscene.h"
#include "paintfield/core/layeredit.h"
#include "paintfield/core/canvas.h"
#include "paintfield/core/workspace.h"
#include "paintfield/core/palettemanager.h"
#include "paintfield/core/tabletevent.h"
#include "paintfield/core/widgets/simplebutton.h"
#include "paintfield/core/rasterlayer.h"

#include "brushstroker.h"

#include "brushtool.h"


using namespace Malachite;

namespace PaintField {

BrushTool::BrushTool(Canvas *parent) :
	Tool(parent),
    _commitTimer(new QTimer(this))
{
	setCursor(QCursor(QPixmap(":/icons/32x32/tinycursor.png")));
	_commitTimer->setInterval(200);
	_commitTimer->setSingleShot(true);
	connect(_commitTimer, SIGNAL(timeout()), this, SLOT(commitStroke()));
}

BrushTool::~BrushTool() {}

void BrushTool::drawLayer(SurfacePainter *painter, const LayerConstRef &layer)
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
	
	if (event->modifiers() == Qt::ShiftModifier)
	{
		if (_lastEndData)
		{
			auto beginData = *_lastEndData;
			beginData.pressure = event->data.pressure;
			beginStroke(beginData);
			drawStroke(event->data);
			endStroke(event->data);
		}
		else
		{
			_lastEndData = boost::make_optional(event->data);
		}
	}
}

void BrushTool::tabletMoveEvent(CanvasTabletEvent *event)
{
	PAINTFIELD_DEBUG << "press" << "pressure:" << event->data.pressure;
	
	if (event->data.pressure && !_isStroking)
	{
		beginStroke(event->data);
	}
	else if (_isStroking)
		drawStroke(event->data);
	
	event->accept();
}

void BrushTool::tabletReleaseEvent(CanvasTabletEvent *event)
{
	PAINTFIELD_DEBUG << "release" << "pressure:" << event->data.pressure;
	endStroke(event->data);
	event->accept();
}

void BrushTool::beginStroke(const TabletInputData &data)
{
	if (!_strokerFactory)
		return;
	
	PAINTFIELD_DEBUG << "begin";
	
	_commitTimer->stop();
	
	if (!_stroker || _layer != currentLayer())
	{
		commitStroke();
		
		setEditing(true);
		
		_layer = dynamicSPCast<const RasterLayer>(currentLayer());
		
		if (!_layer || _layer->isLocked())
			return;
		
		_surface = _layer->surface();
		
		_stroker.reset(_strokerFactory->createStroker(&_surface));
		_stroker->loadSettings(_settings);
		_stroker->setPixel(_pixel);
		_stroker->setRadiusBase(double(_brushSize) * 0.5);
		_stroker->setSmoothed(_smoothEnabled);
		
		addLayerDelegation(_layer);
	}
	
	_isStroking = true;
	_stroker->moveTo(data);
}

void BrushTool::drawStroke(const TabletInputData &data)
{
	if (!_isStroking)
		return;
	
	_stroker->lineTo(data);
	
	updateTiles();
}

void BrushTool::endStroke(const TabletInputData &data)
{
	if (!_isStroking)
		return;
	
	PAINTFIELD_DEBUG << "end";
	
	_stroker->end();
	
	updateTiles();
	
	_lastEndData = boost::make_optional(data);
	
	_isStroking = false;
	_commitTimer->start();
}

void BrushTool::commitStroke()
{
	if (!_stroker)
		return;
	
	_surface.squeeze(_stroker->totalEditedKeys());
	
	canvas()->viewController()->setUpdateTilesEnabled(false);
	canvas()->document()->layerScene()->editLayer(_layer, new LayerSurfaceEdit(_surface, _stroker->totalEditedKeys()), tr("Brush"));
	canvas()->viewController()->setUpdateTilesEnabled(true);
	
	_stroker.reset();
	clearLayerDelegation();
	PAINTFIELD_DEBUG << "commit editing";
	
	setEditing(false);
}

void BrushTool::updateTiles()
{
	if (!_stroker || _stroker->lastEditedKeysWithRects().isEmpty())
		return;
	
	emit requestUpdate(_stroker->lastEditedKeysWithRects());
	_stroker->clearLastEditedKeys();
}

void BrushTool::setBrushSettings(const QVariantMap &settings)
{
	_settings = settings;
}

}
