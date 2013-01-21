#include <QtGui>

#include "util.h"
#include "appcontroller.h"
#include "toolmanager.h"
#include "palettemanager.h"
#include "module.h"
#include "modulemanager.h"
#include "canvassplitareacontroller.h"
#include "debug.h"

#include "workspacecontroller.h"

namespace PaintField
{

struct WorkspaceController::Data
{
	QList<CanvasController *> canvasControllers;
	QPointer<CanvasController> currentCanvas;
	
	ToolManager *toolManager = 0;
	PaletteManager *paletteManager = 0;
	
	QActionList actions;
	WorkspaceModuleList modules;
	
	QActionList nullCanvasActions;
	CanvasModuleList nullCanvasModules;
	
	ScopedQObjectPointer<WorkspaceView> view;
};


WorkspaceController::WorkspaceController(QObject *parent) :
    QObject(parent),
    d(new Data)
{
	d->toolManager = new ToolManager(this);
	d->paletteManager = new PaletteManager(this);
	
	QVariantMap workspaceItemOrderMap = appController()->workspaceItemOrder().toMap();
	
	{
		auto view = new WorkspaceView(this, 0);
		
		{
			auto controller = new CanvasSplitAreaController(view, this);
			
			connect(this, SIGNAL(canvasShowRequested(CanvasController*)), controller, SLOT(addCanvas(CanvasController*)));
			
			d->actions << createAction("paintfield.view.splitVertically", controller, SLOT(splitCurrentVertically()));
			d->actions << createAction("paintfield.view.splitHorizontally", controller, SLOT(splitCurrentHorizontally()));
			d->actions << createAction("paintfield.view.closeCurrentSplit", controller, SLOT(closeCurrent()));
			
			view->setCentralWidget(controller->view());
		}
		
		connect(this, SIGNAL(currentCanvasChanged(CanvasController*)), view, SLOT(setCurrentCanvas(CanvasController*)));
		connect(view, SIGNAL(closeRequested()), this, SLOT(tryClose()));
		
		view->createSideBarFrames(appController()->sideBarDeclarationHash(), workspaceItemOrderMap["sidebars"]);
		view->createToolBars(appController()->toolBarDeclarationHash(), workspaceItemOrderMap["toolbars"]);
		view->createMenuBar(appController()->actionDeclarationHash(), appController()->menuDeclarationHash(), appController()->menuBarOrder());
		
		d->view.reset(view);
	}
	
	d->actions << createAction("paintfield.file.new", this, SLOT(newCanvas()));
	d->actions << createAction("paintfield.file.open", this, SLOT(openCanvas()));
	d->actions << createAction("paintfield.file.newFromImageFile", this, SLOT(newCanvasFromImageFile()));
}

WorkspaceController::~WorkspaceController()
{
	delete d;
}

ToolManager *WorkspaceController::toolManager()
{
	return d->toolManager;
}

PaletteManager *WorkspaceController::paletteManager()
{
	return d->paletteManager;
}

WorkspaceView *WorkspaceController::view()
{
	return d->view.data();
}

void WorkspaceController::updateView()
{
	updateWorkspaceItems();
	updateWorkspaceItemsForCanvas(d->currentCanvas);
	updateMenuBar();
}

void WorkspaceController::addModules(const QList<WorkspaceModule *> &modules)
{
	for (auto module : modules)
		addActions(module->actions());
	
	d->modules += modules;
}

WorkspaceModuleList WorkspaceController::modules()
{
	return d->modules;
}

void WorkspaceController::addActions(const QActionList &actions)
{
	d->actions += actions;
}

QActionList WorkspaceController::actions()
{
	return d->actions;
}

void WorkspaceController::addNullCanvasModules(const CanvasModuleList &modules)
{
	for (auto module : modules)
		addNullCanvasActions(module->actions());
	
	d->nullCanvasModules += modules;
}

CanvasModuleList WorkspaceController::nullCanvasModules()
{
	return d->nullCanvasModules;
}

void WorkspaceController::addNullCanvasActions(const QActionList &actions)
{
	d->nullCanvasActions += actions;
}

QActionList WorkspaceController::nullCanvasActions()
{
	return d->nullCanvasActions;
}

void WorkspaceController::newCanvas()
{
	auto canvas = CanvasController::fromNew();
	if (canvas)
	{
		addAndShowCanvas(canvas);
		setCurrentCanvas(canvas);
	}
}

void WorkspaceController::newCanvasFromImageFile()
{
	auto canvas = CanvasController::fromNewFromImageFile();
	if (canvas)
	{
		addAndShowCanvas(canvas);
		setCurrentCanvas(canvas);
	}
}

void WorkspaceController::openCanvas()
{
	auto canvas = CanvasController::fromOpen();
	if (canvas)
	{
		addAndShowCanvas(canvas);
		setCurrentCanvas(canvas);
	}
}

void WorkspaceController::openCanvasFromFilepath(const QString &filepath)
{
	auto canvas = CanvasController::fromFile(filepath);
	if (canvas)
	{
		addAndShowCanvas(canvas);
		setCurrentCanvas(canvas);
	}
}

bool WorkspaceController::tryClose()
{
	for (CanvasController *canvas : d->canvasControllers)
	{
		if (!canvas->closeCanvas())
			return false;
	}
	emit shouldBeDeleted(this);
	return true;
}

void WorkspaceController::setFocus()
{
	d->view->setFocus();
}

void WorkspaceController::setCurrentCanvas(CanvasController *canvas)
{
	if (d->currentCanvas != canvas)
	{
		d->currentCanvas = canvas;
		if (canvas)
			canvas->onSetCurrent();
		
		PAINTFIELD_DEBUG << "current canvas changed:" << canvas;
		emit currentCanvasChanged(canvas);
		
		updateWorkspaceItemsForCanvas(canvas);
		updateMenuBar();
	}
}

bool WorkspaceController::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == d->view.data())
	{
		if (event->type() == QEvent::FocusIn)
		{
			emit focused();
			return true;
		}
	}
	
	return false;
}

void WorkspaceController::addAndShowCanvas(CanvasController *canvas)
{
	addCanvas(canvas);
	emit canvasShowRequested(canvas);
}

void WorkspaceController::addCanvas(CanvasController *canvas)
{
	if (!canvas)
		return;
	
	if (canvas->workspace() && canvas->workspace() != this)
		canvas->workspace()->removeCanvas(canvas);
	
	if (!d->canvasControllers.contains(canvas))
	{
		canvas->setWorkspace(this);
		d->canvasControllers << canvas;
		connect(canvas, SIGNAL(shouldBeDeleted(CanvasController*)), this, SLOT(deleteCanvas(CanvasController*)));
	}
}

void WorkspaceController::removeCanvas(CanvasController *canvas)
{
	if (d->canvasControllers.contains(canvas))
	{
		d->canvasControllers.removeAll(canvas);
		disconnect(canvas, 0, this, 0);
		canvas->setWorkspace(0);
	}
}

QList<CanvasController *> WorkspaceController::canvases()
{
	return d->canvasControllers;
}

void WorkspaceController::deleteCanvas(CanvasController *canvas)
{
	if (d->canvasControllers.contains(canvas))
	{
		if (d->currentCanvas == canvas)
			setCurrentCanvas(0);
		
		removeCanvas(canvas);
		canvas->deleteLater();
	}
}

QActionList WorkspaceController::currentCanvasActions()
{
	return d->currentCanvas ? d->currentCanvas->actions() : d->nullCanvasActions;
}

CanvasModuleList WorkspaceController::currentCanvasModules()
{
	return d->currentCanvas ? d->currentCanvas->modules() : d->nullCanvasModules;
}

void WorkspaceController::updateWorkspaceItems()
{
	for (const QString &name : appController()->sidebarNames())
	{
		QWidget *sidebar = sideBarForWorkspace(appController()->modules(), modules(), name);
		if (sidebar)
			view()->setSidebar(name, sidebar);
	}
	
	for (const QString &name : appController()->toolbarNames())
	{
		QToolBar *toolBar = view()->toolBar(name);
		if (toolBar)
			updateToolBar(appController()->modules(), modules(), currentCanvasModules(), toolBar, name);
	}
}

void WorkspaceController::updateWorkspaceItemsForCanvas(CanvasController *canvas)
{
	Q_UNUSED(canvas)
	
	for (const QString &name : appController()->sidebarNames())
	{
		QWidget *sidebar = sideBarForCanvas(currentCanvasModules(), name);
		if (sidebar)
			view()->setSidebar(name, sidebar);
	}
	
	for (const QString &name : appController()->toolbarNames())
	{
		QToolBar *toolBar = view()->toolBar(name);
		if (toolBar)
			updateToolBar(AppModuleList(), WorkspaceModuleList(), currentCanvasModules(), toolBar, name);
	}
}

void WorkspaceController::updateMenuBar()
{
	QActionList actions = appController()->actions() + this->actions() + currentCanvasActions();
	view()->associateMenuBarWithActions(actions);
}

}
