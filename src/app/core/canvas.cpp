#include <QtGui>

#include "application.h"
#include "document.h"
#include "dialogs/newdocumentdialog.h"
#include "dialogs/exportdialog.h"
#include "drawutil.h"
#include "tool.h"
#include "mlimageio.h"

#include "canvas.h"

namespace PaintField
{

using namespace Malachite;

class FSCanvasRenderer : public LayerRenderer
{
public:
	FSCanvasRenderer() : LayerRenderer(), _tool(0) {}
	
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
	
	Tool *_tool;
};

CanvasGraphicsObject::CanvasGraphicsObject(Document *document, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    _document(document),
    _tool(0),
    _pixmap(document->size())
{
	updateTiles(document->tileKeys());
	connect(_document, SIGNAL(tilesUpdated(QPointSet)), this, SLOT(updateTiles(QPointSet)));
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
	connect(tool, SIGNAL(requestUpdate(QPointSet)), this, SLOT(updateTiles(QPointSet)));
}

void CanvasGraphicsObject::updateTiles(const QPointSet &tiles)
{
	QElapsedTimer timer;
	timer.start();
	
	QPointSet renderTiles = tiles & _document->tileKeys();
	
	FSCanvasRenderer renderer;
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
	
	return QGraphicsObject::sceneEvent(event);
}

class FSCanvasItem : public QGraphicsItem
{
	friend class CanvasScene;
};

CanvasScene::CanvasScene(QObject *parent) :
    QGraphicsScene(parent),
    _cursorItem(0)
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
		{
			_cursorItem = item;
		}
		if ((int)event->type() == PaintField::EventTabletRelease)
		{
			_cursorItem = 0;
		}
		
		tabletEvent->data.pos = item->mapFromScene(pos);
		return item->sceneEvent(tabletEvent);
	}
	else
	{
		event->ignore();
		return false;
	}
}

Canvas::Canvas(Document *document, QWidget *parent) :
    QGraphicsView(parent),
    _document(document),
    _mousePressure(0)
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
	
	connect(app()->toolManager(), SIGNAL(currentToolFactoryChanged(ToolFactory*)), this, SLOT(updateTool()));
	updateTool();
	
	connect(_document, SIGNAL(modifiedChanged(bool)), this, SLOT(setWindowModified(bool)));
	connect(_document, SIGNAL(filePathChanged(QString)), this, SLOT(documentPathChanged(QString)));
	documentPathChanged(_document->filePath());
	
	// setting default transformation
	
	setTransformationAnchor(QGraphicsView::NoAnchor);
	setTransform(QTransform::fromTranslate(-document->width(), -document->height()));
	
	// creating actions (deleted when the canvas deleted)
	
	app()->actionManager()->addAction(new QAction(this), "saveDocument", this, SLOT(saveCanvas()));
	app()->actionManager()->addAction(new QAction(this), "saveAsAction", this, SLOT(saveAsCanvas()));
	app()->actionManager()->addAction(new QAction(this), "closeDocument", this, SLOT(closeCanvas()));
}

Canvas::~Canvas() {}

void Canvas::changeCanvasSize(const QSize &size)
{
	//QRect rect(-size.width() - size.width()/2, -size.height() - size.height()/2, 3 * size.width(), 3 * size.height());
	QRect rect(-size.width(), -size.height(), size.width() * 3, size.height() * 3);
	setSceneRect(rect);
}

void Canvas::updateTool()
{
	ToolFactory *factory = app()->toolManager()->currentToolFactory();
	_tool.reset(factory ? factory->createTool(this) : 0);
	_canvasGraphicsObject->setTool(_tool.data());
}

void Canvas::documentPathChanged(const QString &path)
{
	if (path.isEmpty())
		setWindowFilePath(_document->tempName());
	else
		setWindowFilePath(_document->filePath());
}

void Canvas::repaintCanvas(const QRect &rect)
{
	QRect viewRect = viewportTransform().mapRect(QRectF(rect)).toAlignedRect();
	repaint(viewRect);
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
	if (processAsTabletEvent(event))
		return;
	
	QGraphicsView::mouseMoveEvent(event);
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (processAsTabletEvent(event))
			return;
	}
	
	QGraphicsView::mousePressEvent(event);
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (processAsTabletEvent(event))
			return;
	}
	
	QGraphicsView::mouseReleaseEvent(event);
}

void Canvas::fsTabletEvent(TabletEvent *event)
{
	event->data.pos *= viewportTransform().inverted();
	canvasScene()->event(event);
}

bool Canvas::processAsTabletEvent(QMouseEvent *event)
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

void Canvas::wheelEvent(QWheelEvent *event)
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

bool Canvas::event(QEvent *event)
{
	switch ((int)event->type()) {
	case PaintField::EventTabletMove:
	case PaintField::EventTabletPress:
	case PaintField::EventTabletRelease:
		fsTabletEvent(static_cast<TabletEvent *>(event));
		return event->isAccepted();
	default:
		return QGraphicsView::event(event);
	}
}

Canvas *Canvas::newCanvas()
{
	NewDocumentDialog dialog;
	if (dialog.exec() != QDialog::Accepted) {
		return 0;
	}
	
	RasterLayer *layer = new RasterLayer(tr("Untitled Layer"));
	
	Document *document = new Document(tr("Untitled"), dialog.documentSize(), layer);
	return new Canvas(document);
}

Canvas *Canvas::openCanvas()
{
	QString filePath = QFileDialog::getOpenFileName(0,
	                                                tr("Open"),
	                                                QDir::homePath(),
	                                                tr("PaintField Project (*.pfproj)"));
	if (filePath.isEmpty())	// cancelled
		return 0;
	Document *document = Document::open(filePath);
	if (document == 0) {	// failed to open
		QMessageBox::warning(0, QString(), tr("Failed to open file."));
	}
	return new Canvas(document);
}

bool Canvas::saveAsCanvas()
{
	Document *document = this->document();
	
	QString filePath = QFileDialog::getSaveFileName(0,
	                                                tr("Save As"),
	                                                QDir::homePath(),
	                                                tr("PaintField Project (*.pfproj)"));
	if (filePath.isEmpty())
		return false;
	
	QFileInfo fileInfo(filePath);
	QFileInfo dirInfo(fileInfo.dir().path());
	if (!dirInfo.isWritable())
	{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setWindowTitle(QString());
		messageBox.setText(tr("The specified folder is not writable."));
		messageBox.setInformativeText(tr("Save in another folder."));
		messageBox.exec();
		return false;
	}
	
	if (!document->saveAs(filePath))
	{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setWindowTitle(QString());
		messageBox.setText(tr("Failed to save the file."));
		messageBox.exec();
		return false;
	}
	return true;
}

bool Canvas::saveCanvas()
{
	Document *document = this->document();
	
	// first save
	if (document->fileName().isEmpty())
	{
		return saveAsCanvas();
	}
	
	// file is not modified
	if (!document->isModified())
		return true;
	
	if (!document->save())
	{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setWindowTitle(tr("Error"));
		messageBox.setText(tr("Cannot save file"));
		messageBox.exec();
		return false;
	}
	return true;
}

bool Canvas::closeCanvas()
{
	Document *document = this->document();
	
	if (document->isModified())
	{
		QMessageBox messageBox;
		messageBox.setText(tr("Do you want to save your changes?"));
		messageBox.setInformativeText(tr("The changes will be lost if you don't save them."));
		messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		messageBox.setDefaultButton(QMessageBox::Save);
		int ret = messageBox.exec();
		
		switch (ret)
		{
			case QMessageBox::Save:
				if (!saveCanvas())
				return false;
				break;
			case QMessageBox::Discard:
				break;
			case QMessageBox::Cancel:
				return false;
			default:
				Q_ASSERT(0);
				break;
		}
	}
	
	deleteLater();
	
	return true;
}

}

