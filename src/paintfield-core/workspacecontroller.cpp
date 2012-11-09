#include <QtGui>

#include "util.h"
#include "application.h"
#include "toolmanager.h"
#include "palettemanager.h"
#include "module.h"
#include "modulemanager.h"

#include "workspacecontroller.h"

namespace PaintField
{






WorkspaceController::WorkspaceController(QObject *parent) :
    QObject(parent)
{
	// create tool / palette manager
	
	_toolManager = new ToolManager(this);
	_paletteManager = new PaletteManager(this);
	
	//_canvasTabAreaController = new SplitTabAreaController(this);
	
	//connect(this, SIGNAL(canvasAdded(CanvasController*)), _canvasTabAreaController, SLOT(addCanvas(CanvasController*)));
	//connect(this, SIGNAL(canvasAboutToBeRemoved(CanvasController*)), _canvasTabAreaController, SLOT(removeCanvas(CanvasController*)));
	//connect(this, SIGNAL(currentCanvasChanged(CanvasController*)), _canvasTabAreaController, SLOT(setCurrentCanvas(CanvasController*)));
	
	//connect(_canvasTabAreaController, SIGNAL(currentCanvasChanged(CanvasController*)), this, SLOT(setCurrentCanvas(CanvasController*)));
	
	_actions << createAction("paintfield.file.new", this, SLOT(newCanvas()));
	_actions << createAction("paintfield.file.open", this, SLOT(openCanvas()));
}

WorkspaceView *WorkspaceController::createView(QWidget *parent)
{
	WorkspaceView *view = new WorkspaceView(parent);
	_view.reset(view);
	
	connect(view, SIGNAL(closeRequested()), this, SLOT(tryClose()));
	
	//view->setCentralWidget(_canvasTabAreaController->createView(view));
	
	QVariantMap workspaceItemOrderMap = app()->workspaceItemOrder().toMap();
	
	view->createSideBarFrames(app()->sideBarDeclarationHash(), workspaceItemOrderMap["sidebars"]);
	view->createToolBars(app()->toolBarDeclarationHash(), workspaceItemOrderMap["toolbars"]);
	view->createMenuBar(app()->actionDeclarationHash(), app()->menuDeclarationHash(), app()->menuBarOrder());
	
	updateWorkspaceItems();
	updateWorkspaceItemsForCanvas(_currentCanvas);
	updateMenuBar();
	
	return view;
}

void WorkspaceController::addModules(const QList<WorkspaceModule *> &modules)
{
	for (auto module : modules)
		addActions(module->actions());
	
	_modules += modules;
}

void WorkspaceController::addNullCanvasModules(const CanvasModuleList &modules)
{
	for (auto module : modules)
		addNullCanvasActions(module->actions());
	
	_nullCanvasModules += modules;
}

void WorkspaceController::newCanvas()
{
	CanvasController *controller = CanvasController::fromNew(this);
	
	if (controller)
	{
		addCanvas(controller);
		setCurrentCanvas(controller);
	}
}

void WorkspaceController::openCanvas()
{
	CanvasController *controller = CanvasController::fromOpen(this);
	
	if (controller)
	{
		addCanvas(controller);
		setCurrentCanvas(controller);
	}
}

bool WorkspaceController::tryClose()
{
	for (CanvasController *canvas : _canvasControllers)
	{
		if (!canvas->closeCanvas())
			return false;
	}
	emit shouldBeDeleted(this);
	return true;
}

void WorkspaceController::setFocus()
{
	_view->setFocus();
}

void WorkspaceController::setCurrentCanvas(CanvasController *canvas)
{
	if (_currentCanvas != canvas)
	{
		_currentCanvas = canvas;
		emit currentCanvasChanged(canvas);
		
		if (_view)
		{
			updateWorkspaceItemsForCanvas(canvas);
			updateMenuBar();
		}
	}
}

bool WorkspaceController::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == _view.data())
	{
		if (event->type() == QEvent::FocusIn)
		{
			emit focused();
			return true;
		}
	}
	
	return false;
}

void WorkspaceController::addCanvas(CanvasController *canvas)
{
	_canvasControllers << canvas;
	connect(canvas, SIGNAL(shouldBeDeleted(CanvasController*)), this, SLOT(removeCanvas(CanvasController*)));
	canvas->addModules(app()->moduleManager()->createCanvasModules(canvas, canvas));
	
	emit canvasAdded(canvas);
}

void WorkspaceController::removeCanvas(CanvasController *canvas)
{
	if (_canvasControllers.contains(canvas))
	{
		if (_currentCanvas == canvas)
			setCurrentCanvas(0);
		
		_canvasControllers.removeAll(canvas);
		emit canvasAboutToBeRemoved(canvas);
		canvas->deleteLater();
	}
}

void WorkspaceController::updateWorkspaceItems()
{
	for (const QString &name : app()->sidebarNames())
	{
		QWidget *sidebar = createSideBarForWorkspace(app()->modules(), modules(), name);
		if (sidebar)
			_view->setSidebar(name, sidebar);
	}
	
	for (const QString &name : app()->toolbarNames())
	{
		QToolBar *toolBar = _view->toolBar(name);
		if (toolBar)
			updateToolBar(app()->modules(), modules(), currentCanvasModules(), toolBar, name);
	}
}

void WorkspaceController::updateWorkspaceItemsForCanvas(CanvasController *canvas)
{
	Q_UNUSED(canvas)
	
	for (const QString &name : app()->sidebarNames())
	{
		QWidget *sidebar = createSideBarForCanvas(currentCanvasModules(), name);
		if (sidebar)
			_view->setSidebar(name, sidebar);
	}
	
	for (const QString &name : app()->toolbarNames())
	{
		QToolBar *toolBar = _view->toolBar(name);
		if (toolBar)
			updateToolBar(AppModuleList(), WorkspaceModuleList(), currentCanvasModules(), toolBar, name);
	}
}

void WorkspaceController::updateMenuBar()
{
	QActionList actions = app()->actions() + this->actions() + currentCanvasActions();
	_view->associateMenuBarWithActions(actions);
}

}
