#include "canvascontroller.h"
#include "canvasview.h"
#include "widgets/splittabareacontroller.h"

#include "workspacecanvasareacontroller.h"

namespace PaintField
{

WorkspaceCanvasAreaController::WorkspaceCanvasAreaController(QWidget *baseWindow, QObject *parent) :
	QObject(parent),
	_tabArea(new SplitTabAreaController(baseWindow, this))
{
	connect(_tabArea, SIGNAL(currentTabChanged(QWidget*)), this, SLOT(onCurrentTabChanged(QWidget*)));
}

QWidget *WorkspaceCanvasAreaController::view()
{
	return _tabArea->view();
}

void WorkspaceCanvasAreaController::addCanvas(CanvasController *controller)
{
	_tabArea->addTab(controller->view(), controller->document()->fileName());
}

void WorkspaceCanvasAreaController::removeCanvas(CanvasController *controller)
{
	_tabArea->removeTab(controller->view());
}

void WorkspaceCanvasAreaController::setCurrentCanvas(CanvasController *controller)
{
	_tabArea->setCurrentTab(controller ? controller->view() : 0);
}

void WorkspaceCanvasAreaController::split(Qt::Orientation orientation)
{
	_tabArea->split(orientation);
}

void WorkspaceCanvasAreaController::closeCurrentSplit()
{
	_tabArea->closeCurrentSplit();
}

void WorkspaceCanvasAreaController::onCurrentTabChanged(QWidget *tab)
{
	CanvasView *view = qobject_cast<CanvasView *>(tab);
	if (view)
		emit currentCanvasChanged(view->controller());
}

}


