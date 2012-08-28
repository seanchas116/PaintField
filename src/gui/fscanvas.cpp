#include <QtGui>

#include "fsguimain.h"
#include "fsrasterlayer.h"
#include "fsnewdocumentdialog.h"
#include "fsexportdialog.h"
#include "fsdrawutil.h"
#include "fstool.h"
#include "mlimageio.h"

#include "fscanvas.h"

FSCanvas *FSCanvas::newFile()
{
	FSNewDocumentDialog dialog;
	if (dialog.exec() != QDialog::Accepted) {
		return 0;
	}
	
	FSRasterLayer *layer = new FSRasterLayer(tr("Untitled Layer"));
	
	FSDocumentModel *document = new FSDocumentModel(tr("Untitled"), dialog.documentSize(), layer);
	
	return new FSCanvas(document);
}

FSCanvas *FSCanvas::openFile()
{
	QString filePath = QFileDialog::getOpenFileName(0,
	                                                QObject::tr("Open"),
	                                                QDir::homePath(),
	                                                QObject::tr("PaintField Project (*.pfproj)"));
	if (filePath.isEmpty())	// cancelled
		return 0;
	FSDocumentModel *document = FSDocumentModel::open(filePath);
	if (document == 0) {	// failed to open
		QMessageBox::warning(0, QString(), QObject::tr("Failed to open file."));
		return 0;
	}
	return new FSCanvas(document);
}

bool FSCanvas::closeFile()
{
	if (document()->isModified()) {
		QMessageBox messageBox;
		messageBox.setText(QObject::tr("Do you want to save your changes?"));
		messageBox.setInformativeText(QObject::tr("The changes will be lost if you don't save them."));
		messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		messageBox.setDefaultButton(QMessageBox::Save);
		int ret = messageBox.exec();
		
		switch (ret) {
		case QMessageBox::Save:
			if (!saveFile())
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

bool FSCanvas::saveFile()
{
	// first save
	if (document()->fileName().isEmpty()) {
		return saveAsFile();
	}
	
	// file is not modified
	if (!document()->isModified())
		return true;
	
	if (!document()->save()) {
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setWindowTitle(QObject::tr("Error"));
		messageBox.setText(QObject::tr("Cannot save file"));
		messageBox.exec();
		return false;
	}
	return true;
}

bool FSCanvas::saveAsFile()
{
	QString filePath = QFileDialog::getSaveFileName(0,
	                                                tr("Save As"),
	                                                QDir::homePath(),
	                                                tr("PaintField Project (*.pfproj)"));
	if (filePath.isEmpty())
		return false;
	
	QFileInfo fileInfo(filePath);
	QFileInfo dirInfo(fileInfo.dir().path());
	if (!dirInfo.isWritable()) {
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setWindowTitle(QString());
		messageBox.setText(QObject::tr("The specified folder is not writable."));
		messageBox.setInformativeText(QObject::tr("Save in another folder."));
		messageBox.exec();
		return false;
	}
	
	if (!document()->saveAs(filePath)) {
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setWindowTitle(QString());
		messageBox.setText(QObject::tr("Failed to save the file."));
		messageBox.exec();
		return false;
	}
	return true;
}

bool FSCanvas::exportFile()
{
	FSExportDialog dialog;
	if (dialog.exec() != QDialog::Accepted)
		return false;
	
	QString filePath = QFileDialog::getSaveFileName(0,
	                                                QObject::tr("Export"),
	                                                QDir::homePath(),
	                                                dialog.currentText());
	if (filePath.isEmpty())
		return false;	// cancelled
	
	MLImage image(document()->size());
	image.fill(MLColor::white().toArgb());
	MLPainter painter(&image);
	document()->render(&painter);
	painter.end();
	
	MLImageExporter exporter(image, dialog.currentFormat());
	bool result = exporter.save(filePath, dialog.currentQuality());
	
	if (!result)
		QMessageBox::warning(0, QString(), QObject::tr("Failed to export file."));
	
	return result;
}





FSCanvasGraphicsObject::FSCanvasGraphicsObject(FSDocumentModel *document, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    _document(document),
    _tool(0),
    _pixmap(document->size())
{
	updateTiles(document->tileKeys());
	connect(_document, SIGNAL(tilesUpdated(QPointSet)), this, SLOT(updateTiles(QPointSet)));
}

void FSCanvasGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	painter->drawPixmap(0, 0, _pixmap);
}

void FSCanvasGraphicsObject::setTool(FSTool *tool)
{
	_tool = tool;
	connect(tool, SIGNAL(requestUpdate(QPointSet)), this, SLOT(updateTiles(QPointSet)));
}

void FSCanvasGraphicsObject::updateTiles(const QPointSet &tiles)
{
	QElapsedTimer timer;
	timer.start();
	
	QPointSet renderTiles = tiles & _document->tileKeys();
	
	FSLayerRenderer renderer;
	renderer.setDelegate(_tool ? _tool->renderDelegate() : 0);
	
	foreach (const QPoint &key, renderTiles)
	{
		MLImage tile = renderer.renderTile(_document->layerForIndex(QModelIndex()), key, MLSurface::WhiteTile);
		
		QPainter painter(&_pixmap);
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		
		fsDrawMLImageFast(&painter, key * MLSurface::TileSize, tile);
		
		painter.end();
		
		QRect rect(key * MLSurface::TileSize, tile.size());
		emit requireRepaint(rect);
	}
	
	qDebug() << Q_FUNC_INFO << "took" << timer.elapsed() << "ms";
}

void FSCanvasGraphicsObject::changeCanvasSize(const QSize &size)
{
	_pixmap = QPixmap(size);
	updateTiles(MLSurface::keysForRect(QRect(QPoint(), size)));
}

bool FSCanvasGraphicsObject::sceneEvent(QEvent *event)
{
	switch ((int)event->type())
	{
		case FSGlobal::EventTabletMove:
			_tool->cursorMoveEvent(static_cast<FSTabletEvent *>(event));
			return event->isAccepted();
		case FSGlobal::EventTabletPress:
			_tool->cursorPressEvent(static_cast<FSTabletEvent *>(event));
			return event->isAccepted();
		case FSGlobal::EventTabletRelease:
			_tool->cursorReleaseEvent(static_cast<FSTabletEvent *>(event));
			return event->isAccepted();
		default:
			break;
	}
	
	return QGraphicsObject::sceneEvent(event);
}

class FSCanvasItem : public QGraphicsItem
{
	friend class FSCanvasScene;
};

FSCanvasScene::FSCanvasScene(QObject *parent) :
    QGraphicsScene(parent)
{
}

bool FSCanvasScene::event(QEvent *event)
{
	switch ((int)event->type())
	{
	case FSGlobal::EventTabletMove:
	case FSGlobal::EventTabletPress:
	case FSGlobal::EventTabletRelease:
		break;
	default:
		return QGraphicsScene::event(event);
	}
	
	FSTabletEvent *tabletEvent = static_cast<FSTabletEvent *>(event);
	MLVec2D pos = tabletEvent->data.pos;
	FSCanvasItem *item = static_cast<FSCanvasItem *>(itemAt(pos));
	
	if (item) 
	{
		tabletEvent->data.pos = item->mapFromScene(pos);
		return item->sceneEvent(tabletEvent);
	}
	else
	{
		event->ignore();
		return false;
	}
}

FSCanvas::FSCanvas(FSDocumentModel *document, QWidget *parent) :
    QGraphicsView(parent),
    _document(document),
    _mousePressure(0)
{
	document->setParent(this);
	
	setScene(new FSCanvasScene(this));
	setMouseTracking(true);
	
	setBackgroundBrush(Qt::lightGray);
	
	changeCanvasSize(_document->size());
	
	_canvasGraphicsObject = new FSCanvasGraphicsObject(document);
	scene()->addItem(_canvasGraphicsObject);
	connect(_canvasGraphicsObject, SIGNAL(requireRepaint(QRect)), this, SLOT(repaintCanvas(QRect)));
	
	connect(fsToolManager(), SIGNAL(currentToolFactoryChanged(FSToolFactory*)), this, SLOT(updateTool()));
	updateTool();
	
	connect(_document, SIGNAL(modifiedChanged(bool)), this, SLOT(setWindowModified(bool)));
	connect(_document, SIGNAL(filePathChanged(QString)), this, SLOT(documentPathChanged(QString)));
	documentPathChanged(_document->filePath());
	
	setTransformationAnchor(QGraphicsView::NoAnchor);
	setTransform(QTransform::fromTranslate(-document->width(), -document->height()));
}

FSCanvas::~FSCanvas() {}

void FSCanvas::changeCanvasSize(const QSize &size)
{
	//QRect rect(-size.width() - size.width()/2, -size.height() - size.height()/2, 3 * size.width(), 3 * size.height());
	QRect rect(-size.width(), -size.height(), size.width() * 3, size.height() * 3);
	setSceneRect(rect);
}

void FSCanvas::updateTool()
{
	FSToolFactory *factory = fsToolManager()->currentToolFactory();
	_tool.reset(factory ? factory->createTool(this) : 0);
	_canvasGraphicsObject->setTool(_tool.data());
}

void FSCanvas::documentPathChanged(const QString &path)
{
	if (path.isEmpty())
		setWindowFilePath(_document->tempName());
	else
		setWindowFilePath(_document->filePath());
}

void FSCanvas::repaintCanvas(const QRect &rect)
{
	QRect viewRect = viewportTransform().mapRect(QRectF(rect)).toAlignedRect();
	repaint(viewRect);
}

void FSCanvas::mouseMoveEvent(QMouseEvent *event)
{
	if (processAsTabletEvent(event))
		return;
	
	QGraphicsView::mouseMoveEvent(event);
}

void FSCanvas::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (processAsTabletEvent(event))
			return;
	}
	
	QGraphicsView::mousePressEvent(event);
}

void FSCanvas::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (processAsTabletEvent(event))
			return;
	}
	
	QGraphicsView::mouseReleaseEvent(event);
}

void FSCanvas::fsTabletEvent(FSTabletEvent *event)
{
	event->data.pos *= viewportTransform().inverted();
	canvasScene()->event(event);
}

bool FSCanvas::processAsTabletEvent(QMouseEvent *event)
{
	int type;
	
	switch (event->type())
	{
		default:
		case QEvent::MouseMove:
			type = FSGlobal::EventTabletMove;
			break;
		case QEvent::MouseButtonPress:
			type = FSGlobal::EventTabletPress;
			_mousePressure = 1.0;
			break;
		case QEvent::MouseButtonRelease:
			type = FSGlobal::EventTabletRelease;
			_mousePressure = 0.0;
			break;
	}
	
	QScopedPointer<FSTabletEvent> tabletEvent(new FSTabletEvent(type, event->globalPos(), event->globalPos(), event->posF() * viewportTransform().inverted(), _mousePressure, 0, 0, 0, 0, event->modifiers()));
	tabletEvent->setAccepted(false);
	canvasScene()->event(tabletEvent.data());
	
	return tabletEvent->isAccepted();
}

void FSCanvas::wheelEvent(QWheelEvent *event)
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

void FSCanvas::focusInEvent(QFocusEvent *)
{
	emit windowFocusIn();
}

void FSCanvas::closeEvent(QCloseEvent *event)
{
	if (closeFile())
	{
		event->accept();
		emit windowClosed();
	}
	else
	{
		event->ignore();
	}
}

bool FSCanvas::event(QEvent *event)
{
	switch ((int)event->type()) {
	case FSGlobal::EventTabletMove:
	case FSGlobal::EventTabletPress:
	case FSGlobal::EventTabletRelease:
		fsTabletEvent(static_cast<FSTabletEvent *>(event));
		return event->isAccepted();
	default:
		return QGraphicsView::event(event);
	}
}

