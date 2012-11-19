#include <QtGui>

#include "util.h"
#include "appcontroller.h"
#include "toolmanager.h"
#include "palettemanager.h"
#include "module.h"
#include "modulemanager.h"
#include "workspacecanvasareacontroller.h"

#include "workspacecontroller.h"

namespace PaintField
{






WorkspaceController::WorkspaceController(QObject *parent) :
    QObject(parent),
	_toolManager(new ToolManager(this)),
	_paletteManager(new PaletteManager(this)),
	_view(new WorkspaceView),
	_canvasAreaController(new WorkspaceCanvasAreaController(_view.data(), this))
{
	connect(this, SIGNAL(canvasAdded(CanvasController*)), _canvasAreaController, SLOT(addCanvas(CanvasController*)));
	connect(this, SIGNAL(canvasAboutToBeRemoved(CanvasController*)), _canvasAreaController, SLOT(removeCanvas(CanvasController*)));
	connect(this, SIGNAL(currentCanvasChanged(CanvasController*)), _canvasAreaController, SLOT(setCurrentCanvas(CanvasController*)));
	connect(_canvasAreaController, SIGNAL(currentCanvasChanged(CanvasController*)), this, SLOT(setCurrentCanvas(CanvasController*)));
	
	_actions << createAction("paintfield.file.new", this, SLOT(newCanvas()));
	_actions << createAction("paintfield.file.open", this, SLOT(openCanvas()));
	
	connect(_view.data(), SIGNAL(closeRequested()), this, SLOT(tryClose()));
	_view->setCentralWidget(_canvasAreaController->view());
	
	QVariantMap workspaceItemOrderMap = appController()->workspaceItemOrder().toMap();
	
	_view->createSideBarFrames(appController()->sideBarDeclarationHash(), workspaceItemOrderMap["sidebars"]);
	_view->createToolBars(appController()->toolBarDeclarationHash(), workspaceItemOrderMap["toolbars"]);
	_view->createMenuBar(appController()->actionDeclarationHash(), appController()->menuDeclarationHash(), appController()->menuBarOrder());
}

void WorkspaceController::updateView()
{
	updateWorkspaceItems();
	updateWorkspaceItemsForCanvas(_currentCanvas);
	updateMenuBar();
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
		
		updateWorkspaceItemsForCanvas(canvas);
		updateMenuBar();
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
	canvas->addModules(appController()->moduleManager()->createCanvasModules(canvas, canvas));
	
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
	for (const QString &name : appController()->sidebarNames())
	{
		QWidget *sidebar = createSideBarForWorkspace(appController()->modules(), modules(), name);
		if (sidebar)
			_view->setSidebar(name, sidebar);
	}
	
	for (const QString &name : appController()->toolbarNames())
	{
		QToolBar *toolBar = _view->toolBar(name);
		if (toolBar)
			updateToolBar(appController()->modules(), modules(), currentCanvasModules(), toolBar, name);
	}
}

void WorkspaceController::updateWorkspaceItemsForCanvas(CanvasController *canvas)
{
	Q_UNUSED(canvas)
	
	for (const QString &name : appController()->sidebarNames())
	{
		QWidget *sidebar = createSideBarForCanvas(currentCanvasModules(), name);
		if (sidebar)
			_view->setSidebar(name, sidebar);
	}
	
	for (const QString &name : appController()->toolbarNames())
	{
		QToolBar *toolBar = _view->toolBar(name);
		if (toolBar)
			updateToolBar(AppModuleList(), WorkspaceModuleList(), currentCanvasModules(), toolBar, name);
	}
}

void WorkspaceController::updateMenuBar()
{
	QActionList actions = appController()->actions() + this->actions() + currentCanvasActions();
	_view->associateMenuBarWithActions(actions);
}

}
