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

void WorkspaceCanvasAreaController::addCanvas(CanvasController *canvas)
{
	_tabArea->addTab(canvas->view(), canvas->document()->fileName());
}

void WorkspaceCanvasAreaController::removeCanvas(CanvasController *canvas)
{
	_tabArea->removeTab(canvas->view());
}

void WorkspaceCanvasAreaController::setCurrentCanvas(CanvasController *canvas)
{
	if (_currentCanvas)
	{
		disconnect(_currentCanvas->document(), SIGNAL(filePathChanged(QString)), this, SLOT(onCurrentCanvasPropertyChanged()));
	}
	
	_currentCanvas = canvas;
	_tabArea->setCurrentTab(canvas ? canvas->view() : 0);
	
	if (_currentCanvas)
	{
		connect(_currentCanvas->document(), SIGNAL(filePathChanged(QString)), this, SLOT(onCurrentCanvasPropertyChanged()));
	}
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

void WorkspaceCanvasAreaController::onCurrentCanvasPropertyChanged()
{
	auto tabWidget = _tabArea->tabWidgetForTab(_currentCanvas->view());
	tabWidget->setTabText(tabWidget->indexOf(_currentCanvas->view()), _currentCanvas->document()->fileName());
}

}


