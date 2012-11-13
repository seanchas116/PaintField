#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QGraphicsView>
#include <QGraphicsSceneEvent>
#include <QGraphicsObject>
#include "document.h"

/*
namespace PaintField
{

class Tool;
class TabletEvent;
class CanvasController;

namespace Obsolete
{

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
	
	Document *_document = 0;
	Tool *_tool = 0;
	QPixmap _pixmap;
};

class CanvasScene : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit CanvasScene(QObject *parent = 0) : QGraphicsScene(parent) {}
	bool event(QEvent *event);
	
public slots:
	
private:
	QGraphicsItem *_cursorItem = 0;
};

class CanvasView : public QGraphicsView
{
	Q_OBJECT
public:
	
	typedef QGraphicsView super;
	
	explicit CanvasView(Document *document, CanvasController *controller, QWidget *parent = 0);
	
	~CanvasView();
	
	Document *document() { return _document; }
	
	CanvasController *controller() { return _controller; }
	
signals:
	
public slots:
	
	void setTool(const QString &name);
	
protected:
	
	void keyPressEvent(QKeyEvent *event);
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
	
	void changeCanvasSize(const QSize &size);
	CanvasScene *canvasScene() { return static_cast<CanvasScene *>(scene() ); }
	
	bool processAsTabletEvent(QMouseEvent *event);
	
	CanvasGraphicsObject *_canvasGraphicsObject = 0;
	Document *_document = 0;
	CanvasController *_controller = 0;
	double _mousePressure = 0.0;
};

}

}
*/

#endif // CANVASVIEW_H
