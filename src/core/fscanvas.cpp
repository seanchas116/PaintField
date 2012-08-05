#include <QtGui>
#include <QThread>

#include "fsscopedtimer.h"
#include "fsthumbnail.h"
#include "fstool.h"
#include "mlblendmode.h"
#include "fsnewdocumentdialog.h"
#include "mlpainter.h"
#include "fsdrawutil.h"
#include "fslayerrenderer.h"

#include "fscanvas.h"

FSCanvasGraphicsObject::FSCanvasGraphicsObject(const QSize &size, FSCanvas *canvas, QGraphicsItem *parent)
    : QGraphicsObject(parent),
      size(size),
      pixmap(size),
      canvas(canvas)
{
	pixmap.fill(Qt::transparent);
}

bool FSCanvasGraphicsObject::sceneEvent(QEvent *event)
{
	return QCoreApplication::sendEvent(canvas, event);
}

void FSCanvasGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
	painter->drawPixmap(QPoint(), pixmap);
}

/*
class FSCanvasRenderThread : public QThread
{
	Q_OBJECT
	
public:
	
	FSCanvasRenderThread(QPixmap *pixmap, FSDocumentModel *document, FSTool *tool, QObject *parent = 0) : 
		QThread(parent),
		_abort(false),
		_pixmap(pixmap),
		_document(document),
		_tool(tool)
	{}
	
	~FSCanvasRenderThread()
	{
		_mutex.lock();
		_abort = true;
		_condition.wakeOne();
		_mutex.unlock();
		
		wait();
	}
	
	void run();
	void updateView(const QPoint &key);
	
	void safeTerminate();
	
public slots:
	
	void enqueueTile(const QPoint &key);
	
signals:
	
	void viewUpdated(const QRect &rect);
	
private:
	
	QMutex _mutex;
	QWaitCondition _condition;
	bool _abort;
	QQueue<QPoint> _queue;
	
	QPixmap *_pixmap;
	FSDocumentModel *_document;
	FSTool *_tool;
};

void FSCanvasRenderThread::run()
{
	forever
	{
		_mutex.lock();
		
		if (_abort)
		{
			_mutex.unlock();
			return;
		}
		
		if (_queue.isEmpty())
		{
			_condition.wait(&_mutex);
			_mutex.unlock();
			continue;
		}
		
		QPoint key = _queue.dequeue();
		_mutex.unlock();
		
		_document->mutex()->lock();
		updateView(key);
		_document->mutex()->unlock();
	}
}

void FSCanvasRenderThread::updateView(const QPoint &key)
{
	FSLayerRenderer renderer;
	renderer.setDelegate(_tool);
	
	MLImage tile = MLSurface::WhiteTile;
	MLPainter tilePainter(&tile);
	
	renderer.render(&tilePainter, _document->layerForIndex(QModelIndex()), key);
	tilePainter.flush();
	
	QPainter painter(_pixmap);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	
	fsDrawMLImage(&painter, key * MLSurface::TileSize, tile);
	
	painter.end();
	
	emit viewUpdated(MLSurface::keyToRect(key));
}

void FSCanvasRenderThread::enqueueTile(const QPoint &key)
{
	_mutex.lock();
	
	if (_queue.contains(key))
	{
		_mutex.unlock();
		return;
	}
	
	_queue.enqueue(key);
	
	_condition.wakeOne();
	_mutex.unlock();
}*/


FSCanvas::FSCanvas(FSDocumentModel *document, QObject *parent)
    : QObject(parent),
      _document(document),
      _canvasGraphicsObject(new FSCanvasGraphicsObject(_document->size(), this ) ),
	  _tool(0)
{
	Q_ASSERT(document);
	
	document->setParent(this);
	
	connect(document, SIGNAL(tilesUpdated(QPointSet)), this, SLOT(updateView(QPointSet)));
	connect(document, SIGNAL(modified()), this, SLOT(updateThumbnail()));
	
	updateView();
	updateThumbnail();
}


void FSCanvas::updateView(const QPointSet &tiles)
{
	FSScopedTimer timer(__PRETTY_FUNCTION__);
	
	QPointSet renderTiles = tiles & _document->tileKeys();
	qDebug() << Q_FUNC_INFO << ": rendering" << renderTiles.size() << "tiles";
	
	FSLayerRenderer renderer;
	renderer.setDelegate(tool());
	
	foreach (const QPoint &key, renderTiles) {
		MLImage tile = MLSurface::WhiteTile;
		MLPainter tilePainter(&tile);
		
		renderer.render(&tilePainter, documentModel()->layerForIndex(QModelIndex()), key);
		tilePainter.flush();
		
		QPainter painter(&(_canvasGraphicsObject->pixmap));
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		
		fsDrawMLImageFast(&painter, key * MLSurface::TileSize, tile);
		
		painter.end();
		
		emit viewUpdated(MLSurface::keyToRect(key));
	}
}

void FSCanvas::updateThumbnail()
{
	_thumbnail = FSThumbnail::createThumbnail(pixmap());
	emit thumbnailUpdated();
}

