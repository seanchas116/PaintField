#ifndef WORKSPACEVIEW_H
#define WORKSPACEVIEW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "canvasview.h"

namespace PaintField
{

class CanvasMdiSubWindow : public QMdiSubWindow
{
	Q_OBJECT
public:
	CanvasMdiSubWindow(CanvasView *canvas, QWidget *parent);
	
	CanvasView *canvas() { return _canvas; }
	
signals:
	
	void closeRequested(CanvasView *canvas);
	
	void windowHidden(CanvasMdiSubWindow *swindow);
	
protected:
	
	void closeEvent(QCloseEvent *closeEvent);
	void changeEvent(QEvent *changeEvent);
	
private:
	
	CanvasView *_canvas;
};

class CanvasMdiArea : public QMdiArea
{
	Q_OBJECT
public:
	explicit CanvasMdiArea(QWidget *parent = 0);
	
signals:
	
	void currentCanvasChanged(CanvasView *canvas);
	void canvasVisibleChanged(CanvasView *canvas, bool visible);
	
	void canvasCloseRequested(CanvasView *canvas);
	
public slots:
	
	void addCanvas(CanvasView *canvas);
	void removeCanvas(CanvasView *canvas);
	void setCanvasVisible(CanvasView *canvas, bool visible);
	void setCurrentCanvas(CanvasView *canvas);
	
private slots:
	
	void onSubWindowActivated(QMdiSubWindow *swindow);
	void onSubWindowHidden(CanvasMdiSubWindow *swindow);
	
private:
	
	CanvasMdiSubWindow *subWindowForCanvas(CanvasView *canvas);
	
	QList<CanvasMdiSubWindow *> _subWindows;
};

class WorkspaceView : public QMainWindow
{
	Q_OBJECT
public:
	explicit WorkspaceView(QWidget *parent = 0);
	
	void addPanel(Qt::DockWidgetArea area, QWidget *panel);
	QWidgetList panels() { return _panels; }
	
signals:
	
public slots:
	
private:
	
	QWidgetList _panels;
};

}

#endif // WORKSPACEVIEW_H
