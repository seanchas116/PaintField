#ifndef FSCANVASVIEW_H
#define FSCANVASVIEW_H

#include <QGraphicsView>
#include "fscanvas.h"

class FSTabletEvent;

class FSCanvasScene : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit FSCanvasScene(QObject *parent = 0) : QGraphicsScene(parent) {}
	bool event(QEvent *event);
};

class FSCanvasView : public QGraphicsView
{
	Q_OBJECT
public:
	explicit FSCanvasView(FSCanvas *canvas, QWidget *parent = 0);
	~FSCanvasView();
	
	FSCanvas *canvas() { return _canvas; }
	FSCanvasScene *canvasScene() { return static_cast<FSCanvasScene *>(scene() ); }
	FSDocumentModel *documentModel() { return _canvas->documentModel(); }
	
	static FSCanvasView *newFile();
	static FSCanvasView *openFile();
	bool closeFile();
	bool saveFile();
	bool saveAsFile();
	
signals:
	
	void windowFocusIn();
	void windowClosed();
	
public slots:
	
protected:
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void fsTabletEvent(FSTabletEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);
	void focusInEvent(QFocusEvent *);
	void closeEvent(QCloseEvent *);
	
	bool event(QEvent *event);
	
private slots:
	
	void canvasUpdated(const QRect &rect);
	void updateTool();
	void documentPathChanged(const QString &path);
	
private:
	
	FSCanvas *_canvas;
	QScopedPointer<FSTool> _tool;
	qreal _mousePressure;
};

#endif // FSCANVASVIEW_H
