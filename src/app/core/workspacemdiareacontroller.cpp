#include <QtGui>

#include "canvasview.h"

#include "workspacemdiareacontroller.h"

namespace PaintField
{

WorkspaceMdiSubWindow::WorkspaceMdiSubWindow(CanvasController *canvas, QWidget *parent) :
    QMdiSubWindow(parent),
	_canvas(canvas)
{
	setWidget(canvas->createView());
}

void WorkspaceMdiSubWindow::closeEvent(QCloseEvent *closeEvent)
{
	_canvas->closeCanvas();
	closeEvent->ignore();
}

void WorkspaceMdiSubWindow::changeEvent(QEvent *changeEvent)
{
	/*
	if (changeEvent->type() == QEvent::WindowStateChange)
	{
		if (!(windowState() && Qt::WindowMinimized))
		{
			setVisible(false);
			emit windowHidden(this);
		}
	}*/
	QMdiSubWindow::changeEvent(changeEvent);
}



WorkspaceMdiAreaController::WorkspaceMdiAreaController(QObject *parent) :
	QObject(parent)
{}

QMdiArea *WorkspaceMdiAreaController::createView(QWidget *parent)
{
	_mdiArea = new QMdiArea(parent);
	connect(_mdiArea.data(), SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(onSubWindowActivated(QMdiSubWindow*)));
	return _mdiArea;
}

void WorkspaceMdiAreaController::addCanvas(CanvasController *controller)
{
	WorkspaceMdiSubWindow *swindow = new WorkspaceMdiSubWindow(controller, _mdiArea);
	_subWindows << swindow;
	swindow->show();
}

void WorkspaceMdiAreaController::removeCanvas(CanvasController *controller)
{
	WorkspaceMdiSubWindow *swindow = subWindowForCanvas(controller);
	if (swindow)
	{
		_subWindows.removeOne(swindow);
		swindow->deleteLater();
	}
	else
		qWarning() << Q_FUNC_INFO << ": invalid canvas";
}

void WorkspaceMdiAreaController::setCanvasVisible(CanvasController *controller, bool visible)
{
	WorkspaceMdiSubWindow *swindow = subWindowForCanvas(controller);
	
	if (swindow && swindow->isVisible() != visible)
	{
		swindow->setVisible(visible);
		emit canvasVisibleChanged(controller, visible);
	}
	else
		qWarning() << Q_FUNC_INFO << ": invalid canvas";
}

void WorkspaceMdiAreaController::setCurrentCanvas(CanvasController *controller)
{
	WorkspaceMdiSubWindow *swindow = subWindowForCanvas(controller);
	
	if (swindow)
	{
		if (_mdiArea && _mdiArea->currentSubWindow() != swindow)
			_mdiArea->setActiveSubWindow(swindow);
	}
}

void WorkspaceMdiAreaController::onSubWindowActivated(QMdiSubWindow *swindow)
{
	WorkspaceMdiSubWindow *wswindow = static_cast<WorkspaceMdiSubWindow *>(swindow);
	if (_subWindows.contains(wswindow))
		emit currentCanvasChanged(wswindow->canvas());
}

void WorkspaceMdiAreaController::onSubWindowHidden(WorkspaceMdiSubWindow *swindow)
{
	if (_subWindows.contains(swindow))
		emit canvasVisibleChanged(swindow->canvas(), false);
}

WorkspaceMdiSubWindow *WorkspaceMdiAreaController::subWindowForCanvas(CanvasController *controller)
{
	for (WorkspaceMdiSubWindow *swindow : _subWindows)
	{
		if (swindow->canvas() == controller)
			return swindow;
	}
	return 0;
}

}
