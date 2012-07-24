#include <QtGui>

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
	
	FSLayerRenderer renderer;
	renderer.setDelegate(_tool);
	
	foreach (const QPoint &key, renderTiles) {
		MLImage tile = MLSurface::WhiteTile;
		MLPainter tilePainter(&tile);
		
		renderer.render(&tilePainter, documentModel()->layerForIndex(QModelIndex()), key);
		tilePainter.flush();
		
		QPainter painter(&(_canvasGraphicsObject->pixmap));
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		
		fsDrawMLImage(&painter, key * MLSurface::TileSize, tile);
		
		painter.end();
		
		emit viewUpdated(MLSurface::keyToRect(key));
	}
}

void FSCanvas::updateThumbnail()
{
	_thumbnail = FSThumbnail::createThumbnail(pixmap());
	emit thumbnailUpdated();
}

