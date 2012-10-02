#ifndef FSCANVAS_H
#define FSCANVAS_H

#include <QGraphicsView>
#include <QGraphicsObject>
#include <QGraphicsSceneEvent>
#include "document.h"

namespace PaintField {

class ToolFactory;
class Tool;
class TabletEvent;

class CanvasGraphicsObject : public QGraphicsObject
{
	Q_OBJECT
public:
	
	CanvasGraphicsObject(Document *document, QGraphicsItem *parent = 0);
	
	QRectF boundingRect() const { return QRect(QPoint(), _pixmap.size()); }
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	
	void setTool(Tool *tool);
	
signals:
	
	void requireRepaint(const QRect &rect);
	
public slots:
	
	void updateTiles(const QPointSet &keys);
	void updateTiles() { updateTiles(_document->tileKeys()); }
	void changeCanvasSize(const QSize &size);
	
protected:
	
	bool sceneEvent(QEvent *event);
	
private:
	
	Document *_document;
	Tool *_tool;
	QPixmap _pixmap;
};

class CanvasScene : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit CanvasScene(QObject *parent = 0);
	bool event(QEvent *event);
	
public slots:
	
private:
	QGraphicsItem *_cursorItem;
};

class Canvas : public QGraphicsView
{
	Q_OBJECT
public:
	
	explicit Canvas(Document *document, QWidget *parent = 0);
	~Canvas();
	
	Document *document() { return _document; }
	CanvasScene *canvasScene() { return static_cast<CanvasScene *>(scene() ); }
	
	static Canvas *newCanvas();
	static Canvas *openCanvas();
	
signals:
	
public slots:
	
	void changeCanvasSize(const QSize &size);
	
	bool saveAsCanvas();
	bool saveCanvas();
	bool closeCanvas();
	
	void setToolFactory(ToolFactory *factory);
	
protected:
	
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void fsTabletEvent(TabletEvent *event);
	void wheelEvent(QWheelEvent *event);
	
	bool event(QEvent *event);
	
private slots:
	
	void documentPathChanged(const QString &path);
	void repaintCanvas(const QRect &rect);
	
private:
	
	bool processAsTabletEvent(QMouseEvent *event);
	
	CanvasGraphicsObject *_canvasGraphicsObject;
	Document *_document;
	QScopedPointer<Tool> _tool;
	double _mousePressure;
};

}

#endif // FSCANVAS_H
