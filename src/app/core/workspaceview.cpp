#include <QtGui>

#include "workspaceview.h"

namespace PaintField
{

CanvasMdiSubWindow::CanvasMdiSubWindow(Canvas *canvas, QWidget *parent) :
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

void CanvasMdiArea::addCanvas(Canvas *canvas)
{
	CanvasMdiSubWindow *swindow = new CanvasMdiSubWindow(canvas, this);
	connect(swindow, SIGNAL(closeRequested(Canvas*)), this, SIGNAL(canvasCloseRequested(Canvas*)));
	_subWindows << swindow;
}

void CanvasMdiArea::removeCanvas(Canvas *canvas)
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

void CanvasMdiArea::setCanvasVisible(Canvas *canvas, bool visible)
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

void CanvasMdiArea::setCurrentCanvas(Canvas *canvas)
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

CanvasMdiSubWindow *CanvasMdiArea::subWindowForCanvas(Canvas *canvas)
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

}
