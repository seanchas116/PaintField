#include <QtGui>

#include "core/application.h"
#include "modules/documentUI/documentmanagercontroller.h"

#include "canvasmdiarea.h"

namespace PaintField
{

CanvasMdiSubWindow::CanvasMdiSubWindow(Document *document, QWidget *parent) :
    QMdiSubWindow(parent)
{
	setWidget(new Canvas(document, this));
}

void CanvasMdiSubWindow::closeEvent(QCloseEvent *closeEvent)
{
	mdiArea()->setActiveSubWindow(this);
	
	DocumentManagerController::closeDocument(_canvas->document());
	
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

void CanvasMdiArea::addDocument(Document *document)
{
	_subWindows << new CanvasMdiSubWindow(document, this);
}

void CanvasMdiArea::removeDocument(Document *document)
{
	CanvasMdiSubWindow *swindow = subWindowForDocument(document);
	
	if (swindow)
	{
		swindow->deleteLater();
	}
	else
	{
		qWarning() << Q_FUNC_INFO << ": invalid canvas";
	}
}

void CanvasMdiArea::setDocumentVisible(Document *document, bool visible)
{
	CanvasMdiSubWindow *swindow = subWindowForDocument(document);
	
	if (swindow && swindow->isVisible() != visible)
	{
		swindow->setVisible(visible);
		emit documentVisibleChanged(swindow->document(), visible);
	}
	else
	{
		qWarning() << Q_FUNC_INFO << ": invalid canvas";
	}
}

void CanvasMdiArea::setCurrentDocument(Document *document)
{
	CanvasMdiSubWindow *swindow = subWindowForDocument(document);
	
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
			emit currentDocumentChanged(canvasWindow->document());
			return;
		}
	}
}

void CanvasMdiArea::onSubWindowHidden(CanvasMdiSubWindow *swindow)
{
	emit documentVisibleChanged(swindow->document(), false);
}

CanvasMdiSubWindow *CanvasMdiArea::subWindowForDocument(Document *document)
{
	foreach (CanvasMdiSubWindow *subWindow, _subWindows)
	{
		if (subWindow->document() == document)
			return subWindow;
	}
	return 0;
}

}
