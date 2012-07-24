#ifndef FSCANVAS_H
#define FSCANVAS_H

#include <QGraphicsObject>
#include "fsdocumentmodel.h"

class FSTool;
class FSToolFactory;
class FSCanvas;
class FSCanvasView;
class MLPainter;

class FSCanvasGraphicsObject : public QGraphicsObject
{
	Q_OBJECT
	
	friend class FSCanvas;
public:
	explicit FSCanvasGraphicsObject(const QSize &size, FSCanvas *canvas, QGraphicsItem *parent = 0);
	QRectF boundingRect() const { return QRectF(0, 0, size.width(), size.height()); }
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	
protected:
	bool sceneEvent(QEvent *event);
	
private:
	QSize size;
	QPixmap pixmap;
	FSCanvas *canvas;
};

class FSCanvas : public QObject
{
	Q_OBJECT
	friend class FSCanvasGraphicsObject;
	
public:
	
	explicit FSCanvas(FSDocumentModel *document, QObject *parent = 0);
	
	FSDocumentModel *documentModel() { return _document; }
	QGraphicsItem *graphicsItem() { return _canvasGraphicsObject; }
	QPixmap pixmap() const { return _canvasGraphicsObject->pixmap; }
	QPixmap thumbnail() const { return _thumbnail; }
	
	void setTool(FSTool *tool) { _tool = tool; }
	FSTool *tool() { return _tool; }
	
public slots:
	
	void updateView(const QPointSet &tiles);
	void updateView()
		{ updateView(_document->tileKeys()); }
	
signals:
	
	void viewUpdated(const QRect &rect);
	void thumbnailUpdated();
	
private slots:
	
	void updateThumbnail();
	
private:
	
	void updateTool();
	
	FSDocumentModel *_document;
	FSCanvasGraphicsObject *_canvasGraphicsObject;
	QPixmap _thumbnail;
	FSTool *_tool;
};

#endif // FSCANVAS_H
