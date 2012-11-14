#include <QtGui>

#include "drawutil.h"
#include "tool.h"
#include "layerrenderer.h"
#include "debug.h"
#include "canvascontroller.h"
#include "application.h"

#include "canvasview.h"

namespace PaintField
{

using namespace Malachite;

class CanvasRenderer : public LayerRenderer
{
public:
	CanvasRenderer() : LayerRenderer() {}
	
	void setTool(Tool *tool) { _tool = tool; }
	
protected:
	
	void drawLayer(SurfacePainter *painter, const Layer *layer)
	{
		if (_tool && _tool->customDrawLayers().contains(layer))
			_tool->drawLayer(painter, layer);
		else
			LayerRenderer::drawLayer(painter, layer);
	}
	
private:
	
	Tool *_tool = 0;
};


CanvasViewViewport::CanvasViewViewport(LayerModel *layerModel, QWidget *parent) :
	QWidget(parent),
	_layerModel(layerModel),
	_pixmap(layerModel->document()->size())
{
	connect(_layerModel, SIGNAL(tilesUpdated(QPointSet)), this, SLOT(updateTiles(QPointSet)));
	updateTiles(_layerModel->document()->tileKeys());
	updateTransforms();
	
	int width = qMax(_pixmap.width(), _pixmap.height()) * 3;
	resize(width, width);
}

void CanvasViewViewport::setNavigatorTransform(const QTransform &transform)
{
	_navigatorTransform = transform;
	updateTransforms();
}

void CanvasViewViewport::setTool(Tool *tool)
{
	_tool = tool;
	if (tool)
		connect(tool, SIGNAL(requestUpdate(QPointSet)), this, SLOT(updateTiles(QPointSet)));
}

void CanvasViewViewport::updateTiles(const QPointSet &tiles)
{
	PAINTFIELD_CALC_SCOPE_ELAPSED_TIME;
	
	QPointSet renderTiles = tiles & _layerModel->document()->tileKeys();
	
	CanvasRenderer renderer;
	renderer.setTool(_tool);
	
	Surface surface = renderer.renderToSurface(_layerModel->rootLayer()->children(), renderTiles);
	
	for (const QPoint &key : renderTiles)
	{
		Image tile = Surface::WhiteTile;
		
		if (surface.contains(key))
		{
			Painter painter(&tile);
			painter.drawTransformedImage(QPoint(), surface.tileForKey(key));
		}
		
		QPainter painter(&_pixmap);
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		
		drawMLImageFast(&painter, key * Surface::TileSize, tile);
		
		painter.end();
		
		QRect rect(key * Surface::TileSize, tile.size());
		repaint(_transformFromScene.mapRect(rect));
	}
}

void CanvasViewViewport::updateTransforms()
{
	_transformFromScene = QTransform::fromTranslate(- _pixmap.width() / 2, - _pixmap.height() / 2) * _navigatorTransform * QTransform::fromTranslate(geometry().width() / 2, geometry().height() / 2);
	_transformToScene = _transformFromScene.inverted();
}

void CanvasViewViewport::resizeEvent(QResizeEvent *event)
{
	updateTransforms();
	event->accept();
}

void CanvasViewViewport::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setTransform(_transformFromScene);
	painter.drawPixmap(0, 0, _pixmap);
}






CanvasView::CanvasView(CanvasController *controller, QWidget *parent) :
	QScrollArea(parent),
	_document(controller->document()),
	_controller(controller)
{
	_viewport = new CanvasViewViewport(_document->layerModel());
	setWidget(_viewport);
}

void CanvasView::setTool(const QString &name)
{
	Tool *tool = createTool(app()->modules(), controller()->workspace()->modules(), controller()->modules(), name, this);
	_viewport->setTool(tool);
}


}
