#include <QtGui>

#include "fslayeredit.h"
#include "mlsurfacepainter.h"
#include "fstabletevent.h"
#include "fsbrushtool.h"
#include "fsbrushstroker.h"
#include "fsscopedtimer.h"
#include "fssimplebutton.h"
#include "fsbrushsettingwidget.h"


class FSBrushRenderDelegate : public FSLayerRenderDelegate
{
public:
	
	FSBrushRenderDelegate(MLSurface *surface) :
	    FSLayerRenderDelegate(),
	    _surface(surface)
	{
		setReplacingEnabled(true);
	}
	
	void render(MLPainter *painter, const FSLayer *layer, const QPoint &tileKey)
	{
		Q_UNUSED(layer);
		painter->drawImage(0, 0, _surface->tileForKey(tileKey));
	}
	
	MLImage renderReplacing(const FSLayer *layer, const QPoint &tileKey)
	{
		Q_UNUSED(layer);
		return _surface->tileForKey(tileKey);
	}
	
private:
	
	MLSurface *_surface;
};

FSBrushTool::FSBrushTool(FSCanvas *parent) :
	FSTool(parent),
	_dataPrevSet(false),
	_trailing(false),
    _trailingEnabled(false),
	_brushSetting(0),
	_layer(0)
{
	_delegate.reset(new FSBrushRenderDelegate(&_surface));
}

FSBrushTool::~FSBrushTool() {}

FSLayerRenderDelegate *FSBrushTool::renderDelegate()
{
	return _delegate.data();
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
	_stroker.reset(new FSPenStroker(&_surface, _brushSetting));
	_delegate->addTarget(_layer);
	
	// discard pressure for the 1st time to reduce overshoot
	FSTabletInputData newData = data;
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
	
	//documentModel()->setData(documentModel()->indexForLayer(_layer), QVariant::fromValue(_surface), FSGlobal::RoleSurface, tr("Brush"));
	document()->makeSkipNextUpdate();
	document()->editLayer(document()->indexForLayer(_layer), new FSLayerSurfaceEdit(_surface, _stroker->totalEditedKeys()), tr("Brush"));
	
	_stroker.reset();
	_delegate->clearTargets();
}

void FSBrushTool::updateTiles()
{
	emit requestUpdate(_stroker->lastEditedKeys());
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
	setIcon(fsCreateSimpleIconSet(":/icons/32x32/brush.svg"));
}

FSTool *FSBrushToolFactory::createTool(FSCanvas *parent)
{
	FSBrushTool *tool = new FSBrushTool(parent);
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
