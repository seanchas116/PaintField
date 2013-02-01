#include <QtGui>

#include "smartpointer.h"
#include "util.h"
#include "appcontroller.h"
#include "toolmanager.h"
#include "palettemanager.h"
#include "module.h"
#include "modulemanager.h"
#include "canvascontroller.h"

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
	
	WorkspaceView *view = 0;
};


Workspace::Workspace(QObject *parent) :
    QObject(parent),
    d(new Data)
{
	d->toolManager = new ToolManager(this);
	d->paletteManager = new PaletteManager(this);
	
	d->actions << createAction("paintfield.view.splitVertically", this, SIGNAL(splitVerticallyRequested()));
	d->actions << createAction("paintfield.view.splitHorizontally", this, SIGNAL(splitHorizontallyRequested()));
	d->actions << createAction("paintfield.view.closeCurrentSplit", this, SIGNAL(closeCurrentSplitRequested()));
	
	d->actions << createAction("paintfield.file.new", this, SLOT(newCanvas()));
	d->actions << createAction("paintfield.file.open", this, SLOT(openCanvas()));
	d->actions << createAction("paintfield.file.newFromImageFile", this, SLOT(newCanvasFromImageFile()));
	
	addModules(appController()->moduleManager()->createWorkspaceModules(this, this));
	addNullCanvasModules(appController()->moduleManager()->createCanvasModules(0, this));
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

void Workspace::setView(WorkspaceView *view)
{
	d->view = view;
}

WorkspaceView *Workspace::view()
{
	return d->view;
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
	emit focused();
}

void Workspace::setCurrentCanvas(Canvas *canvas)
{
	if (d->currentCanvas != canvas)
	{
		d->currentCanvas = canvas;
		if (canvas)
			canvas->onSetCurrent();
		
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

Canvas *Workspace::currentCanvas()
{
	return d->currentCanvas;
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

}
