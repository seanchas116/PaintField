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
	Tool(parent)
{
	setCursor(QCursor(QPixmap(":/icons/32x32/tinycursor.png")));
}

BrushTool::~BrushTool() {}

void BrushTool::drawLayer(SurfacePainter *painter, const LayerConstPtr &layer)
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
	PAINTFIELD_DEBUG << "press" << "pressure:" << event->data.pressure;
	
	if (event->data.pressure && !isStroking())
		beginStroke(event->data);
	else if (isStroking())
		drawStroke(event->data);
	
	setPrevData(event->data);
	
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
	
	_layer = std::dynamic_pointer_cast<const RasterLayer>(currentLayer());
	
	if (!_layer || _layer->isLocked())
		return;
	
	_surface = _layer->surface();
	
	_stroker.reset(_strokerFactory->createStroker(&_surface));
	_stroker->loadSettings(_settings);
	_stroker->setPixel(_pixel);
	_stroker->setRadiusBase(double(_brushSize) * 0.5);
	_stroker->setSmoothed(_smoothEnabled);
	
	addLayerDelegation(_layer);
	
	_stroker->moveTo(data);
}

void BrushTool::drawStroke(const TabletInputData &data)
{
	if (!isStroking())
		return;
	
	_stroker->lineTo(data);
	
	updateTiles();
}

void BrushTool::endStroke(const TabletInputData &data)
{
	Q_UNUSED(data);
	
	if (!isStroking())
		return;
	
	_stroker->end();
	
	updateTiles();
	
	if (_layer && _layer == currentLayer())
	{
		_surface.squeeze(_stroker->totalEditedKeys());
		canvas()->viewController()->setUpdateTilesEnabled(false);
		canvas()->document()->layerScene()->editLayer(_layer, new LayerSurfaceEdit(_surface, _stroker->totalEditedKeys()), tr("Brush"));
		canvas()->viewController()->setUpdateTilesEnabled(true);
		PAINTFIELD_DEBUG << "ending editing";
	}
	
	_stroker.reset();
	clearLayerDelegation();
}

void BrushTool::updateTiles()
{
	if (!_stroker || _stroker->lastEditedKeysWithRects().isEmpty())
		return;
	
	emit requestUpdate(_stroker->lastEditedKeysWithRects());
	_stroker->clearLastEditedKeys();
}

void BrushTool::setPrevData(const TabletInputData &data)
{
	_dataPrev = data;
	_dataPrevSet = true;
}

void BrushTool::setBrushSettings(const QVariantMap &settings)
{
	_settings = settings;
}

}
