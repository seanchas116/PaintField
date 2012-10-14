#ifndef WORKSPACEMDIAREACONTROLLER_H
#define WORKSPACEMDIAREACONTROLLER_H

#include <QObject>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "canvascontroller.h"

namespace PaintField
{

class WorkspaceMdiSubWindow : public QMdiSubWindow
{
	Q_OBJECT
public:
	WorkspaceMdiSubWindow(CanvasController *canvas, QWidget *parent);
	
	CanvasController *canvas() { return _canvas; }
	
signals:
	
	void closeRequested(CanvasController *canvas);
	void windowHidden(WorkspaceMdiSubWindow *swindow);
	
protected:
	
	void closeEvent(QCloseEvent *closeEvent);
	void changeEvent(QEvent *changeEvent);
	
private:
	
	CanvasController *_canvas;
};

class WorkspaceMdiAreaController : public QObject
{
	Q_OBJECT
public:
	explicit WorkspaceMdiAreaController(QObject *parent = 0);
	
	QMdiArea *createView(QWidget *parent = 0);
	
signals:
	
	void currentCanvasChanged(CanvasController *controller);
	void canvasVisibleChanged(CanvasController *controller, bool visible);
	void canvasCloseRequested(CanvasController *controller);
	
public slots:
	
	void addCanvas(CanvasController *controller);
	void removeCanvas(CanvasController *controller);
	void setCanvasVisible(CanvasController *controller, bool visible);
	void setCurrentCanvas(CanvasController *controller);
	
private slots:
	
	void onSubWindowActivated(QMdiSubWindow *swindow);
	void onSubWindowHidden(WorkspaceMdiSubWindow *swindow);
	
private:
	
	WorkspaceMdiSubWindow *subWindowForCanvas(CanvasController *controller);
	
	QPointer<QMdiArea> _mdiArea;
	QList<WorkspaceMdiSubWindow *> _subWindows;
};

}

#endif // WORKSPACEMDIAREACONTROLLER_H
