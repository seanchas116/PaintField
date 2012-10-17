#include <QtGui>

#include "application.h"
#include "document.h"
#include "dialogs/newdocumentdialog.h"
#include "dialogs/exportdialog.h"
#include "drawutil.h"
#include "tool.h"
#include "Malachite/mlimageio.h"

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

CanvasGraphicsObject::CanvasGraphicsObject(Document *document, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    _document(document),
    _pixmap(document->size())
{
	updateTiles(document->tileKeys());
	connect(_document->layerModel(), SIGNAL(tilesUpdated(QPointSet)), this, SLOT(updateTiles(QPointSet)));
}

void CanvasGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	painter->drawPixmap(0, 0, _pixmap);
}

void CanvasGraphicsObject::setTool(Tool *tool)
{
	_tool = tool;
	if (tool)
		connect(tool, SIGNAL(requestUpdate(QPointSet)), this, SLOT(updateTiles(QPointSet)));
}

void CanvasGraphicsObject::updateTiles(const QPointSet &tiles)
{
	QElapsedTimer timer;
	timer.start();
	
	QPointSet renderTiles = tiles & _document->tileKeys();
	
	CanvasRenderer renderer;
	renderer.setTool(_tool);
	
	Surface surface = renderer.render(_document->layerModel()->rootLayer()->children(), renderTiles);
	
	foreach (const QPoint &key, renderTiles)
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
		emit requireRepaint(rect);
	}
	
	qDebug() << Q_FUNC_INFO << "took" << timer.elapsed() << "ms";
}

void CanvasGraphicsObject::changeCanvasSize(const QSize &size)
{
	_pixmap = QPixmap(size);
	updateTiles(Surface::keysForRect(QRect(QPoint(), size)));
}

bool CanvasGraphicsObject::sceneEvent(QEvent *event)
{
	if (_tool)
	{
		switch ((int)event->type())
		{
			case PaintField::EventTabletMove:
				_tool->cursorMoveEvent(static_cast<TabletEvent *>(event));
				return event->isAccepted();
			case PaintField::EventTabletPress:
				_tool->cursorPressEvent(static_cast<TabletEvent *>(event));
				return event->isAccepted();
			case PaintField::EventTabletRelease:
				_tool->cursorReleaseEvent(static_cast<TabletEvent *>(event));
				return event->isAccepted();
			default:
				break;
		}
	}
	
	return QGraphicsObject::sceneEvent(event);
}

class FSCanvasItem : public QGraphicsItem
{
	friend class CanvasScene;
};

CanvasScene::CanvasScene(QObject *parent) :
    QGraphicsScene(parent)
{
}

bool CanvasScene::event(QEvent *event)
{
	switch ((int)event->type())
	{
	case PaintField::EventTabletMove:
	case PaintField::EventTabletPress:
	case PaintField::EventTabletRelease:
		break;
	default:
		return QGraphicsScene::event(event);
	}
	
	TabletEvent *tabletEvent = static_cast<TabletEvent *>(event);
	Vec2D pos = tabletEvent->data.pos;
	FSCanvasItem *item = static_cast<FSCanvasItem *>(_cursorItem ? _cursorItem : itemAt(pos));
	
	if (item)
	{
		if ((int)event->type() == PaintField::EventTabletPress)
			_cursorItem = item;
		if ((int)event->type() == PaintField::EventTabletRelease)
			_cursorItem = 0;
		
		tabletEvent->data.pos = item->mapFromScene(pos);
		return item->sceneEvent(tabletEvent);
	}
	else
	{
		event->ignore();
		return false;
	}
}

CanvasView::CanvasView(Document *document, CanvasController *controller, QWidget *parent) :
    QGraphicsView(parent),
    _document(document),
	_controller(controller)
{
	setScene(new CanvasScene(this));
	setMouseTracking(true);
	
	setBackgroundBrush(Qt::lightGray);
	//setAttribute(Qt::WA_TranslucentBackground);
	//viewport()->setAttribute(Qt::WA_TranslucentBackground);
	
	changeCanvasSize(_document->size());
	
	_canvasGraphicsObject = new CanvasGraphicsObject(document);
	scene()->addItem(_canvasGraphicsObject);
	connect(_canvasGraphicsObject, SIGNAL(requireRepaint(QRect)), this, SLOT(repaintCanvas(QRect)));
	
	connect(_document, SIGNAL(modifiedChanged(bool)), this, SLOT(setWindowModified(bool)));
	connect(_document, SIGNAL(filePathChanged(QString)), this, SLOT(documentPathChanged(QString)));
	documentPathChanged(_document->filePath());
	
	// setting default transformation
	
	setTransformationAnchor(QGraphicsView::NoAnchor);
	setTransform(QTransform::fromTranslate(-document->width(), -document->height()));
}

CanvasView::~CanvasView() {}

void CanvasView::changeCanvasSize(const QSize &size)
{
	//QRect rect(-size.width() - size.width()/2, -size.height() - size.height()/2, 3 * size.width(), 3 * size.height());
	QRect rect(-size.width(), -size.height(), size.width() * 3, size.height() * 3);
	setSceneRect(rect);
}

void CanvasView::setToolFactory(ToolFactory *factory)
{
	_tool.reset(factory ? factory->createTool(this) : 0);
	_canvasGraphicsObject->setTool(_tool.data());
}

void CanvasView::documentPathChanged(const QString &path)
{
	if (path.isEmpty())
		setWindowFilePath(_document->tempName());
	else
		setWindowFilePath(_document->filePath());
}

void CanvasView::repaintCanvas(const QRect &rect)
{
	QRect viewRect = viewportTransform().mapRect(QRectF(rect)).toAlignedRect();
	repaint(viewRect);
}

void CanvasView::mouseMoveEvent(QMouseEvent *event)
{
	if (processAsTabletEvent(event))
		return;
	
	QGraphicsView::mouseMoveEvent(event);
}

void CanvasView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (processAsTabletEvent(event))
			return;
	}
	
	QGraphicsView::mousePressEvent(event);
}

void CanvasView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (processAsTabletEvent(event))
			return;
	}
	
	QGraphicsView::mouseReleaseEvent(event);
}

void CanvasView::fsTabletEvent(TabletEvent *event)
{
	event->data.pos *= viewportTransform().inverted();
	canvasScene()->event(event);
}

bool CanvasView::processAsTabletEvent(QMouseEvent *event)
{
	int type;
	
	switch (event->type())
	{
		default:
		case QEvent::MouseMove:
			type = PaintField::EventTabletMove;
			break;
		case QEvent::MouseButtonPress:
			type = PaintField::EventTabletPress;
			_mousePressure = 1.0;
			break;
		case QEvent::MouseButtonRelease:
			type = PaintField::EventTabletRelease;
			_mousePressure = 0.0;
			break;
	}
	
	QScopedPointer<TabletEvent> tabletEvent(new TabletEvent(type, event->globalPos(), event->globalPos(), event->posF() * viewportTransform().inverted(), _mousePressure, 0, 0, 0, 0, event->modifiers()));
	tabletEvent->setAccepted(false);
	canvasScene()->event(tabletEvent.data());
	
	return tabletEvent->isAccepted();
}

void CanvasView::wheelEvent(QWheelEvent *event)
{
	qDebug() << "wheel event";
	qDebug() << "delta :" << event->delta() << event->orientation();
	
	switch (event->orientation())
	{
		case Qt::Horizontal:
			translate(event->delta() / 2, 0);
			break;
		case Qt::Vertical:
			translate(0, event->delta() / 2);
			break;
		default:
			break;
	}
}

bool CanvasView::event(QEvent *event)
{
	switch ((int)event->type())
	{
	case PaintField::EventTabletMove:
	case PaintField::EventTabletPress:
	case PaintField::EventTabletRelease:
		fsTabletEvent(static_cast<TabletEvent *>(event));
		return event->isAccepted();
	default:
		return QGraphicsView::event(event);
	}
}

}

