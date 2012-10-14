#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QGraphicsView>
#include <QGraphicsSceneEvent>
#include "document.h"

namespace PaintField {

class ToolFactory;
class Tool;
class TabletEvent;

class CanvasGraphicsObject;

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

class CanvasView : public QGraphicsView
{
	Q_OBJECT
public:
	
	/**
	 * Constructs a canvas view.
	 * @param document A document the canvas handles
	 * @param parent QWidget parent
	 */
	explicit CanvasView(Document *document, QWidget *parent = 0);
	
	~CanvasView();
	
	/**
	 * @return The document the canvas handles
	 */
	Document *document() { return _document; }
	
signals:
	
public slots:
	
	/**
	 * Sets the factory of the tool the canvas will use.
	 * @param factory The tool factory
	 */
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
	
	void changeCanvasSize(const QSize &size);
	CanvasScene *canvasScene() { return static_cast<CanvasScene *>(scene() ); }
	
	bool processAsTabletEvent(QMouseEvent *event);
	
	CanvasGraphicsObject *_canvasGraphicsObject;
	Document *_document;
	QScopedPointer<Tool> _tool;
	double _mousePressure;
};

}

#endif // CANVASVIEW_H
