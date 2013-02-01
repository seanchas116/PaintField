#include <QtGui>

#include "smartpointer.h"
#include "util.h"
#include "appcontroller.h"
#include "toolmanager.h"
#include "palettemanager.h"
#include "module.h"
#include "modulemanager.h"
#include "canvascontroller.h"

#include "canvassplitareacontroller.h"
#include "workspaceview.h"

#include "workspacecontroller.h"

namespace PaintField
{

struct Workspace::Data
{
	QList<Canvas *> canvasControllers;
	QPointer<Canvas> currentCanvas;
	
	ToolManager *toolManager = 0;
	PaletteManager *paletteManager = 0;
	
	QActionList actions;
	WorkspaceModuleList modules;
	
	QActionList nullCanvasActions;
	CanvasModuleList nullCanvasModules;
	
	ScopedQObjectPointer<WorkspaceView> view;
};


Workspace::Workspace(QObject *parent) :
    QObject(parent),
    d(new Data)
{
	d->toolManager = new ToolManager(this);
	d->paletteManager = new PaletteManager(this);
	
	QVariantMap workspaceItemOrderMap = appController()->settingsManager()->workspaceItemOrder().toMap();
	
	{
		auto view = new WorkspaceView(this, 0);
		
		{
			auto controller = new CanvasSplitAreaController(this, this);
			
			connect(this, SIGNAL(canvasShowRequested(Canvas*)),
			        controller, SLOT(addCanvas(Canvas*)));
			
			d->actions << createAction("paintfield.view.splitVertically", controller, SLOT(splitCurrentVertically()));
			d->actions << createAction("paintfield.view.splitHorizontally", controller, SLOT(splitCurrentHorizontally()));
			d->actions << createAction("paintfield.view.closeCurrentSplit", controller, SLOT(closeCurrent()));
			
			view->setCentralWidget(controller->view());
		}
		
		connect(this, SIGNAL(currentCanvasChanged(Canvas*)),
		        view, SLOT(setCurrentCanvas(Canvas*)));
		connect(view, SIGNAL(closeRequested()), this, SLOT(tryClose()));
		
		view->createSideBarFrames(appController()->settingsManager()->sideBarDeclarationHash(),
		                          workspaceItemOrderMap["sidebars"]);
		view->createToolBars(appController()->settingsManager()->toolBarDeclarationHash(),
		                     workspaceItemOrderMap["toolbars"]);
		view->createMenuBar(appController()->settingsManager()->actionDeclarationHash(),
		                    appController()->settingsManager()->menuDeclarationHash(),
		                    appController()->settingsManager()->menuBarOrder());
		
		d->view.reset(view);
	}
	
	d->actions << createAction("paintfield.file.new", this, SLOT(newCanvas()));
	d->actions << createAction("paintfield.file.open", this, SLOT(openCanvas()));
	d->actions << createAction("paintfield.file.newFromImageFile", this, SLOT(newCanvasFromImageFile()));
	
	addModules(appController()->moduleManager()->createWorkspaceModules(this, this));
	addNullCanvasModules(appController()->moduleManager()->createCanvasModules(0, this));
	
	updateWorkspaceItems();
	updateWorkspaceItemsForCanvas(d->currentCanvas);
	updateMenuBar();
}

Workspace::~Workspace()
{
	delete d;
}

ToolManager *Workspace::toolManager()
{
	return d->toolManager;
}

PaletteManager *Workspace::paletteManager()
{
	return d->paletteManager;
}

WorkspaceView *Workspace::view()
{
	return d->view.data();
}

void Workspace::addModules(const QList<WorkspaceModule *> &modules)
{
	for (auto module : modules)
		addActions(module->actions());
	
	d->modules += modules;
}

WorkspaceModuleList Workspace::modules()
{
	return d->modules;
}

void Workspace::addActions(const QActionList &actions)
{
	d->actions += actions;
}

QActionList Workspace::actions()
{
	return d->actions;
}

void Workspace::addNullCanvasModules(const CanvasModuleList &modules)
{
	for (auto module : modules)
		addNullCanvasActions(module->actions());
	
	d->nullCanvasModules += modules;
}

CanvasModuleList Workspace::nullCanvasModules()
{
	return d->nullCanvasModules;
}

void Workspace::addNullCanvasActions(const QActionList &actions)
{
	d->nullCanvasActions += actions;
}

QActionList Workspace::nullCanvasActions()
{
	return d->nullCanvasActions;
}

void Workspace::newCanvas()
{
	auto canvas = Canvas::fromNew();
	if (canvas)
	{
		addAndShowCanvas(canvas);
		setCurrentCanvas(canvas);
	}
}

void Workspace::newCanvasFromImageFile()
{
	auto canvas = Canvas::fromNewFromImageFile();
	if (canvas)
	{
		addAndShowCanvas(canvas);
		setCurrentCanvas(canvas);
	}
}

void Workspace::openCanvas()
{
	auto canvas = Canvas::fromOpen();
	if (canvas)
	{
		addAndShowCanvas(canvas);
		setCurrentCanvas(canvas);
	}
}

void Workspace::openCanvasFromFilepath(const QString &filepath)
{
	auto canvas = Canvas::fromFile(filepath);
	if (canvas)
	{
		addAndShowCanvas(canvas);
		setCurrentCanvas(canvas);
	}
}

bool Workspace::tryClose()
{
	for (Canvas *canvas : d->canvasControllers)
	{
		if (!canvas->closeCanvas())
			return false;
	}
	emit shouldBeDeleted(this);
	return true;
}

void Workspace::setFocus()
{
	d->view->setFocus();
}

void Workspace::setCurrentCanvas(Canvas *canvas)
{
	if (d->currentCanvas != canvas)
	{
		d->currentCanvas = canvas;
		if (canvas)
			canvas->onSetCurrent();
		
		updateWorkspaceItemsForCanvas(canvas);
		updateMenuBar();
		
		PAINTFIELD_DEBUG << "current canvas changed:" << canvas;
		emit currentCanvasChanged(canvas);
	}
}

void Workspace::addAndShowCanvas(Canvas *canvas)
{
	addCanvas(canvas);
	emit canvasShowRequested(canvas);
}

void Workspace::addCanvas(Canvas *canvas)
{
	if (!canvas)
		return;
	
	if (canvas->workspace() && canvas->workspace() != this)
		canvas->workspace()->removeCanvas(canvas);
	
	if (!d->canvasControllers.contains(canvas))
	{
		canvas->setWorkspace(this);
		d->canvasControllers << canvas;
		connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)),
		        this, SLOT(deleteCanvas(Canvas*)));
	}
}

void Workspace::removeCanvas(Canvas *canvas)
{
	if (d->canvasControllers.contains(canvas))
	{
		d->canvasControllers.removeAll(canvas);
		disconnect(canvas, 0, this, 0);
		canvas->setWorkspace(0);
	}
}

QList<Canvas *> Workspace::canvases()
{
	return d->canvasControllers;
}

void Workspace::deleteCanvas(Canvas *canvas)
{
	if (d->canvasControllers.contains(canvas))
	{
		if (d->currentCanvas == canvas)
			setCurrentCanvas(0);
		
		removeCanvas(canvas);
		canvas->deleteLater();
	}
}

QActionList Workspace::currentCanvasActions()
{
	return d->currentCanvas ? d->currentCanvas->actions() : d->nullCanvasActions;
}

CanvasModuleList Workspace::currentCanvasModules()
{
	return d->currentCanvas ? d->currentCanvas->modules() : d->nullCanvasModules;
}

void Workspace::updateWorkspaceItems()
{
	for (const QString &name : appController()->settingsManager()->sidebarNames())
	{
		QWidget *sidebar = sideBarForWorkspace(appController()->modules(), modules(), name);
		if (sidebar)
			view()->setSidebar(name, sidebar);
	}
	
	for (const QString &name : appController()->settingsManager()->toolbarNames())
	{
		QToolBar *toolBar = view()->toolBar(name);
		if (toolBar)
			updateToolBar(appController()->modules(), modules(), currentCanvasModules(), toolBar, name);
	}
}

void Workspace::updateWorkspaceItemsForCanvas(Canvas *canvas)
{
	Q_UNUSED(canvas)
	
	for (const QString &name : appController()->settingsManager()->sidebarNames())
	{
		QWidget *sidebar = sideBarForCanvas(currentCanvasModules(), name);
		if (sidebar)
			view()->setSidebar(name, sidebar);
	}
	
	for (const QString &name : appController()->settingsManager()->toolbarNames())
	{
		QToolBar *toolBar = view()->toolBar(name);
		if (toolBar)
			updateToolBar(AppModuleList(), WorkspaceModuleList(), currentCanvasModules(), toolBar, name);
	}
}

void Workspace::updateMenuBar()
{
	QActionList actions = appController()->actions() + this->actions() + currentCanvasActions();
	view()->associateMenuBarWithActions(actions);
}

}
