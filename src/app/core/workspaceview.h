#ifndef WORKSPACEVIEW_H
#define WORKSPACEVIEW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "canvas.h"

namespace PaintField
{

class CanvasMdiSubWindow : public QMdiSubWindow
{
	Q_OBJECT
public:
	CanvasMdiSubWindow(Canvas *canvas, QWidget *parent);
	
	Canvas *canvas() { return _canvas; }
	
signals:
	
	void closeRequested(Canvas *canvas);
	
	void windowHidden(CanvasMdiSubWindow *swindow);
	
protected:
	
	void closeEvent(QCloseEvent *closeEvent);
	void changeEvent(QEvent *changeEvent);
	
private:
	
	Canvas *_canvas;
};

class CanvasMdiArea : public QMdiArea
{
	Q_OBJECT
public:
	explicit CanvasMdiArea(QWidget *parent = 0);
	
signals:
	
	void currentCanvasChanged(Canvas *canvas);
	void canvasVisibleChanged(Canvas *canvas, bool visible);
	
	void canvasCloseRequested(Canvas *canvas);
	
public slots:
	
	void addCanvas(Canvas *canvas);
	void removeCanvas(Canvas *canvas);
	void setCanvasVisible(Canvas *canvas, bool visible);
	void setCurrentCanvas(Canvas *canvas);
	
private slots:
	
	void onSubWindowActivated(QMdiSubWindow *swindow);
	void onSubWindowHidden(CanvasMdiSubWindow *swindow);
	
private:
	
	CanvasMdiSubWindow *subWindowForCanvas(Canvas *canvas);
	
	QList<CanvasMdiSubWindow *> _subWindows;
};

class WorkspaceView : public QMainWindow
{
	Q_OBJECT
public:
	explicit WorkspaceView(QWidget *parent = 0);
	
signals:
	
public slots:
	
};

}

#endif // WORKSPACEVIEW_H
