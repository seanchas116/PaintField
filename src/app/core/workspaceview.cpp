#include <QtGui>

#include "workspaceview.h"

namespace PaintField
{

CanvasMdiSubWindow::CanvasMdiSubWindow(CanvasView *canvas, QWidget *parent) :
    QMdiSubWindow(parent)
{
	setWidget(canvas);
}

void CanvasMdiSubWindow::closeEvent(QCloseEvent *closeEvent)
{
	mdiArea()->setActiveSubWindow(this);
	
	emit closeRequested(_canvas);
	
	closeEvent->ignore();
}

void CanvasMdiSubWindow::changeEvent(QEvent *changeEvent)
{
	if (changeEvent->type() == QEvent::WindowStateChange)
	{
		if (!(windowState() && Qt::WindowMinimized))
		{
			setVisible(false);
			emit windowHidden(this);
		}
	}
}

CanvasMdiArea::CanvasMdiArea(QWidget *parent) :
    QMdiArea(parent)
{
	connect(this, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(onSubWindowActivated(QMdiSubWindow*)));
}

void CanvasMdiArea::addCanvas(CanvasView *canvas)
{
	CanvasMdiSubWindow *swindow = new CanvasMdiSubWindow(canvas, this);
	connect(swindow, SIGNAL(closeRequested(CanvasView*)), this, SIGNAL(canvasCloseRequested(CanvasView*)));
	_subWindows << swindow;
	swindow->show();
}

void CanvasMdiArea::removeCanvas(CanvasView *canvas)
{
	CanvasMdiSubWindow *swindow = subWindowForCanvas(canvas);
	
	if (swindow)
	{
		swindow->deleteLater();
	}
	else
	{
		qWarning() << Q_FUNC_INFO << ": invalid canvas";
	}
}

void CanvasMdiArea::setCanvasVisible(CanvasView *canvas, bool visible)
{
	CanvasMdiSubWindow *swindow = subWindowForCanvas(canvas);
	
	if (swindow && swindow->isVisible() != visible)
	{
		swindow->setVisible(visible);
		emit canvasVisibleChanged(swindow->canvas(), visible);
	}
	else
	{
		qWarning() << Q_FUNC_INFO << ": invalid canvas";
	}
}

void CanvasMdiArea::setCurrentCanvas(CanvasView *canvas)
{
	CanvasMdiSubWindow *swindow = subWindowForCanvas(canvas);
	
	if (swindow)
	{
		setActiveSubWindow(swindow);
	}
	else
	{
		qWarning() << Q_FUNC_INFO << ": invalid canvas";
	}
}

void CanvasMdiArea::onSubWindowActivated(QMdiSubWindow *swindow)
{
	foreach (CanvasMdiSubWindow *canvasWindow, _subWindows)
	{
		if (canvasWindow == swindow)
		{
			emit currentCanvasChanged(canvasWindow->canvas());
			return;
		}
	}
}

void CanvasMdiArea::onSubWindowHidden(CanvasMdiSubWindow *swindow)
{
	emit canvasVisibleChanged(swindow->canvas(), false);
}

CanvasMdiSubWindow *CanvasMdiArea::subWindowForCanvas(CanvasView *canvas)
{
	foreach (CanvasMdiSubWindow *subWindow, _subWindows)
	{
		if (subWindow->canvas() == canvas)
			return subWindow;
	}
	return 0;
}

WorkspaceView::WorkspaceView(QWidget *parent) :
    QMainWindow(parent)
{
}

void WorkspaceView::addPanel(Qt::DockWidgetArea area, QWidget *panel)
{
	if (_panels.contains(panel))
		return;
	
	QDockWidget *dwidget = new QDockWidget(panel->windowTitle());
	dwidget->setWidget(panel);
	
	addDockWidget(area, dwidget);
	
	_panels << panel;
}

}
