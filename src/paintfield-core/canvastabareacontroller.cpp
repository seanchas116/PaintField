#include "workspacetabwidget.h"
#include "canvasview.h"
#include "canvascontroller.h"
#include "workspacecontroller.h"

#include "canvastabareacontroller.h"

namespace PaintField
{

CanvasTabAreaController::CanvasTabAreaController(QObject *parent) :
	QObject(parent)
{
}

QWidget *CanvasTabAreaController::createView(WorkspaceView *workspaceView, QWidget *parent)
{
	_area.reset(new TabDocumentArea(new WorkspaceTabWidget(workspaceView, WorkspaceTabWidget::TypeCanvas), parent));
	return _area.data();
}

void CanvasTabAreaController::addCanvas(CanvasController *controller)
{
	auto view = controller->createView();
	_area->addTab(view->windowTitle(), view);
}

void CanvasTabAreaController::removeCanvas(CanvasController *controller)
{
	QList<CanvasView *> viewsToDelete;
	
	for (CanvasView *view : _canvasViews)
	{
		if (view->controller() == controller)
			viewsToDelete << view;
	}
	
	for (CanvasView *view : viewsToDelete)
	{
		_canvasViews.removeAll(view);
		view->deleteLater();
	}
}

void CanvasTabAreaController::setCurrentCanvas(CanvasController *controller)
{
	auto view = canvasViewFromController(controller);
	
	if (!view)
		return;
	
	_area->setCurrentTab(view);
}

void CanvasTabAreaController::onCurrentTabChanged(QWidget *tab)
{
	CanvasView *view = qobject_cast<CanvasView *>(tab);
	if (view && _canvasViews.contains(view))
		emit currentCanvasChanged(view->controller());
}

void CanvasTabAreaController::split(Qt::Orientation orientation)
{
	if (_area)
		_area->splitCurrent(orientation);
}

void CanvasTabAreaController::closeCurrentSplit()
{
	if (_area)
		_area->closeCurrent();
}

CanvasView *CanvasTabAreaController::canvasViewFromController(CanvasController *controller)
{
	for (CanvasView *view : _canvasViews)
	{
		if (view->controller() == controller)
			return view;
	}
	return 0;
}

}
