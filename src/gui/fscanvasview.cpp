#include <QtGui>
#include "fscore.h"
#include "fstabletevent.h"
#include "fsnewdocumentdialog.h"

#include "fscanvasview.h"


class FSCanvasItem : public QGraphicsItem
{
	friend class FSCanvasScene;
};

bool FSCanvasScene::event(QEvent *event)
{
	switch ((int)event->type()) {
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
	if (item) {
		tabletEvent->data.pos = item->mapToScene(pos);
		return item->sceneEvent(tabletEvent);
	} else {
		event->ignore();
		return false;
	}
}

FSCanvasView::FSCanvasView(FSCanvas *canvas, QWidget *parent) :
    QGraphicsView(parent),
    _canvas(canvas),
	_tool(0),
    _mousePressure(0)
{
	Q_ASSERT(canvas);
	
	canvas->setParent(this);
	connect(canvas, SIGNAL(viewUpdated(QRect)), this, SLOT(canvasUpdated(QRect)));
	
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
	setScene(new FSCanvasScene(this));
	setMouseTracking(true);
	
	setBackgroundBrush(QBrush(Qt::lightGray));
	
	canvasScene()->addItem(canvas->graphicsItem());
	
	connect(fsToolManager(), SIGNAL(currentToolFactoryChanged(FSToolFactory*)), this, SLOT(updateTool()));
	updateTool();
	
	connect(documentModel(), SIGNAL(modifiedChanged(bool)), this, SLOT(setWindowModified(bool)));
	connect(documentModel(), SIGNAL(filePathChanged(QString)), this, SLOT(documentPathChanged(QString)));
	documentPathChanged(documentModel()->filePath());
}

FSCanvasView::~FSCanvasView() {}

void FSCanvasView::canvasUpdated(const QRect &rect)
{
	QRect viewRect = viewportTransform().mapRect(QRectF(rect)).toAlignedRect();
	repaint(viewRect);
}

void FSCanvasView::updateTool()
{
	FSToolFactory *factory = fsToolManager()->currentToolFactory();
	_tool.reset(factory ? factory->createTool(this) : 0);
	canvas()->setTool(_tool.data());
}

void FSCanvasView::documentPathChanged(const QString &path)
{
	if (path.isEmpty())
		setWindowFilePath(documentModel()->tempName());
	else
		setWindowFilePath(documentModel()->filePath());
}

void FSCanvasView::mouseMoveEvent(QMouseEvent *event)
{
	_tool->viewCursorMoveEvent(event);
	if (event->isAccepted()) return;
	
	//qDebug() << "mouse move event at" << event->posF().x() << event->posF().y();
	FSTabletEvent *newEvent = new FSTabletEvent(FSGlobal::EventTabletMove, event->globalPos(), event->globalPos(), event->posF() * viewportTransform().inverted(), _mousePressure, 0, 0, 0.0, 0.0, event->modifiers());
	
	_tool->cursorMoveEvent(newEvent);
	if (newEvent->isAccepted()) return;
	
	canvasScene()->event(newEvent);
	if (newEvent->isAccepted()) return;
	
	QGraphicsView::mouseMoveEvent(event);
}

void FSCanvasView::mousePressEvent(QMouseEvent *event)
{
	_tool->viewCursorPressEvent(event);
	if (event->isAccepted()) return;
	
	//qDebug() << "mouse press event at" << event->x() << event->y();
	_mousePressure = 1.0;
	FSTabletEvent *newEvent = new FSTabletEvent(FSGlobal::EventTabletPress, event->globalPos(), event->globalPos(), event->posF() * viewportTransform().inverted(), _mousePressure, 0, 0, 0.0, 0.0, event->modifiers());
	
	_tool->cursorPressEvent(newEvent);
	if (newEvent->isAccepted()) return;
	
	canvasScene()->event(newEvent);
	if (newEvent->isAccepted()) return;
	
	QGraphicsView::mousePressEvent(event);
}

void FSCanvasView::mouseReleaseEvent(QMouseEvent *event)
{
	_tool->viewCursorReleaseEvent(event);
	if (event->isAccepted()) return;
	
	//qDebug() << "mouse release event at" << event->x() << event->y();
	_mousePressure = 0.0;
	FSTabletEvent *newEvent = new FSTabletEvent(FSGlobal::EventTabletRelease, event->globalPos(), event->globalPos(), event->posF() * viewportTransform().inverted(), _mousePressure, 0, 0, 0.0, 0.0, event->modifiers());
	
	_tool->cursorReleaseEvent(newEvent);
	if (newEvent->isAccepted()) return;
	
	canvasScene()->event(newEvent);
	if (newEvent->isAccepted()) return;
	
	QGraphicsView::mouseReleaseEvent(event);
}

void FSCanvasView::fsTabletEvent(FSTabletEvent *event)
{
	//qDebug() << "tablet event at" << event->data.pos << "pressure" << event->data.pressure << "modifier" << (int)event->modifiers();
	event->data.pos = event->data.pos * transform();
	
	switch ((int)event->type())
	{
	case FSGlobal::EventTabletMove:
		_tool->cursorMoveEvent(event);
		break;
	case FSGlobal::EventTabletPress:
		_tool->cursorPressEvent(event);
		break;
	case FSGlobal::EventTabletRelease:
		_tool->cursorReleaseEvent(event);
		break;
	default:
		return;
	}
	
	if (event->isAccepted()) return;
	
	canvasScene()->event(event);
}

void FSCanvasView::keyPressEvent(QKeyEvent *event)
{
	_tool->keyPressEvent(event);
	if (event->isAccepted()) return;
	QGraphicsView::keyPressEvent(event);
}

void FSCanvasView::keyReleaseEvent(QKeyEvent *event)
{
	_tool->keyReleaseEvent(event);
	if (event->isAccepted()) return;
	QGraphicsView::keyReleaseEvent(event);
}

void FSCanvasView::wheelEvent(QWheelEvent *event)
{
	Q_UNUSED(event);
}

void FSCanvasView::focusInEvent(QFocusEvent *)
{
	emit windowFocusIn();
}

void FSCanvasView::closeEvent(QCloseEvent *event)
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

bool FSCanvasView::event(QEvent *event)
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


FSCanvasView *FSCanvasView::newFile()
{
	FSNewDocumentDialog dialog;
	if (dialog.exec() != QDialog::Accepted) {
		return 0;
	}
	FSDocumentModel *document = new FSDocumentModel(tr("Untitled"), dialog.documentSize());
	return new FSCanvasView(new FSCanvas(document));
}

FSCanvasView *FSCanvasView::openFile()
{
	QString filePath = QFileDialog::getOpenFileName(0,
	                                                QObject::tr("Open"),
	                                                QDir::homePath(),
	                                                QObject::tr("PixBrush File (*.pbi)"));
	if (filePath.isEmpty())	// cancelled
		return 0;
	FSDocumentModel *document = FSDocumentModel::open(filePath);
	if (document == 0) {	// failed to open
		QMessageBox::warning(0, QString(), QObject::tr("Failed to open file."));
		return 0;
	}
	return new FSCanvasView(new FSCanvas(document));
}

bool FSCanvasView::closeFile()
{
	if (canvas()->documentModel()->isModified()) {
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

bool FSCanvasView::saveFile()
{
	// first save
	if (canvas()->documentModel()->fileName().isEmpty()) {
		return saveAsFile();
	}
	
	// file is not modified
	if (!canvas()->documentModel()->isModified())
		return true;
	
	if (!canvas()->documentModel()->save()) {
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setWindowTitle(QObject::tr("Error"));
		messageBox.setText(QObject::tr("Cannot save file"));
		messageBox.exec();
		return false;
	}
	return true;
}

bool FSCanvasView::saveAsFile()
{
	QString filePath = QFileDialog::getSaveFileName(0,
	                                                QObject::tr("Save As"),
	                                                QDir::homePath(),
	                                                QObject::tr("PixBrush File (*.pbi)"));
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
	
	if (!canvas()->documentModel()->saveAs(filePath)) {
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setWindowTitle(QString());
		messageBox.setText(QObject::tr("Failed to save the file."));
		messageBox.exec();
		return false;
	}
	return true;
}

