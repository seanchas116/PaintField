#ifndef FSCANVAS_H
#define FSCANVAS_H

#include <QGraphicsView>
#include <QGraphicsObject>
#include <QGraphicsSceneEvent>
#include "fsdocumentmodel.h"

class FSTool;
class FSTabletEvent;

class FSCanvasGraphicsObject : public QGraphicsObject
{
	Q_OBJECT
public:
	
	FSCanvasGraphicsObject(FSDocumentModel *document, QGraphicsItem *parent = 0);
	
	QRectF boundingRect() const { return QRect(QPoint(), _pixmap.size()); }
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	
	void setTool(FSTool *tool);
	
signals:
	
	void requireRepaint(const QRect &rect);
	
public slots:
	
	void updateTiles(const QPointSet &keys);
	void updateTiles() { updateTiles(_document->tileKeys()); }
	void changeCanvasSize(const QSize &size);
	
protected:
	
	bool sceneEvent(QEvent *event);
	
private:
	
	FSDocumentModel *_document;
	FSTool *_tool;
	QPixmap _pixmap;
};

class FSCanvasScene : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit FSCanvasScene(QObject *parent = 0);
	bool event(QEvent *event);
	
public slots:
	
private:
	QGraphicsItem *_cursorItem;
};

class FSCanvas : public QGraphicsView
{
	Q_OBJECT
public:
	
	static FSCanvas *newFile();
	static FSCanvas *openFile();
	bool closeFile();
	bool saveFile();
	bool saveAsFile();
	bool exportFile();
	
	explicit FSCanvas(FSDocumentModel *document, QWidget *parent = 0);
	~FSCanvas();
	
	FSDocumentModel *document() { return _document; }
	FSCanvasScene *canvasScene() { return static_cast<FSCanvasScene *>(scene() ); }
	
signals:
	
	void windowFocusIn();
	void windowClosed();
	
public slots:
	
	void changeCanvasSize(const QSize &size);
	
protected:
	
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void fsTabletEvent(FSTabletEvent *event);
	void wheelEvent(QWheelEvent *event);
	void focusInEvent(QFocusEvent *);
	void closeEvent(QCloseEvent *);
	
	bool event(QEvent *event);
	
private slots:
	
	void updateTool();
	void documentPathChanged(const QString &path);
	void repaintCanvas(const QRect &rect);
	
private:
	
	bool processAsTabletEvent(QMouseEvent *event);
	
	FSCanvasGraphicsObject *_canvasGraphicsObject;
	FSDocumentModel *_document;
	QScopedPointer<FSTool> _tool;
	
	double _mousePressure;
};

#endif // FSCANVAS_H
