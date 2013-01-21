#include <QtGui>

#include "debug.h"
#include "util.h"
#include "workspacemanager.h"
#include "modulemanager.h"
#include "settingsmanager.h"

#include "appcontroller.h"

namespace PaintField
{

struct AppController::Data
{
	Application *app = 0;
	
	WorkspaceManager *workspaceManager = 0;
	ModuleManager *moduleManager = 0;
	SettingsManager *settingsManager = 0;
	
	QList<AppModule *> modules;
	QList<QAction *> actions;
};

AppController::AppController(Application *app, QObject *parent) :
	QObject(parent),
    d(new Data)
{
	d->app = app;
	d->workspaceManager = new WorkspaceManager(this);
	d->moduleManager = new ModuleManager(this);
	d->settingsManager = new SettingsManager(this);
	
	_instance = this;
	
	declareMenus();
	createActions();
	
	connect(app, SIGNAL(fileOpenRequested(QString)), this, SLOT(openFile(QString)));
}

AppController::~AppController()
{
	delete d;
}

void AppController::begin()
{
	d->settingsManager->loadBuiltinSettings();
	d->settingsManager->loadUserSettings();
	
	moduleManager()->initialize(this);
	addModules(moduleManager()->createAppModules(this, this));
	
	workspaceManager()->newWorkspace();
}

WorkspaceManager *AppController::workspaceManager()
{
	return d->workspaceManager;
}

ModuleManager *AppController::moduleManager()
{
	return d->moduleManager;
}

SettingsManager *AppController::settingsManager()
{
	return d->settingsManager;
}

void AppController::addModuleFactory(ModuleFactory *factory)
{
	d->moduleManager->addModuleFactory(factory);
}

void AppController::addModules(const AppModuleList &modules)
{
	for (AppModule *module : modules)
		addActions(module->actions());
	
	d->modules += modules;
}

QList<AppModule *> AppController::modules()
{
	return d->modules;
}

void AppController::addActions(const QList<QAction *> &actions)
{
	d->actions += actions;
}

QList<QAction *> AppController::actions()
{
	return d->actions;
}

QString AppController::unduplicatedNewFileTempName()
{
	return unduplicatedTempName(tr("Untitled"));
}

QString AppController::unduplicatedTempName(const QString &name)
{
	QStringList existingTempNames;
	
	for (WorkspaceController *workspace : workspaceManager()->workspaces())
	{
		for (CanvasController *canvas : workspace->canvases())
		{
			Document *eachDoc = canvas->document();
			
			if (eachDoc->filePath().isEmpty())
				existingTempNames << eachDoc->tempName();
		}
	}
	
	return unduplicatedName(existingTempNames, name);
}

Application *AppController::app()
{
	return d->app;
}

void AppController::handleMessage(const QString &message)
{
	qDebug() << "message:" << message;
}

void AppController::minimizeCurrentWindow()
{
	QWidget *widget = qApp->activeWindow();
	if (widget)
		widget->showMinimized();
}

void AppController::zoomCurrentWindow()
{
	QWidget *widget = qApp->activeWindow();
	if (widget)
		widget->showMaximized();
}

void AppController::openFile(const QString &path)
{
	PAINTFIELD_DEBUG << "file open requested:" << path;
	
	auto workspace = workspaceManager()->currentWorkspace();
	
	if (workspace)
		workspace->openCanvasFromFilepath(path);
}

void AppController::declareMenus()
{
	settingsManager()->declareMenu("paintfield.file",
	                               tr("File"));
	
	settingsManager()->declareAction("paintfield.file.new",
	                                 tr("New..."));
	
	settingsManager()->declareAction("paintfield.file.newFromImageFile",
	                                 tr("New from Image File..."));
	
	settingsManager()->declareAction("paintfield.file.open",
	                                 tr("Open..."));
	settingsManager()->declareAction("paintfield.file.close",
	                                 tr("Close"));
	settingsManager()->declareAction("paintfield.file.save",
	                                 tr("Save"));
	settingsManager()->declareAction("paintfield.file.saveAs",
	                                 tr("Save As..."));
	settingsManager()->declareAction("paintfield.file.export",
	                                 tr("Export..."));
	settingsManager()->declareAction("paintfield.file.quit",
	                                 tr("Quit"));
	
	settingsManager()->declareMenu("paintfield.edit",
	                               tr("Edit"));
	
	settingsManager()->declareAction("paintfield.edit.undo",
	                                 tr("Undo"));
	settingsManager()->declareAction("paintfield.edit.redo",
	                                 tr("Redo"));
	settingsManager()->declareAction("paintfield.edit.cut",
	                                 tr("Cut"));
	settingsManager()->declareAction("paintfield.edit.copy",
	                                 tr("Copy"));
	settingsManager()->declareAction("paintfield.edit.paste",
	                                 tr("Paste"));
	settingsManager()->declareAction("paintfield.edit.delete",
	              
	                                 tr("Delete"));
	settingsManager()->declareAction("paintfield.edit.selectAll",
	                                 tr("Select All"));
	
	settingsManager()->declareMenu("paintfield.view",
	                               tr("View"));
	
	settingsManager()->declareAction("paintfield.view.newWorkspace",
	                                 tr("New Workspace"));
	settingsManager()->declareAction("paintfield.view.splitVertically",
	                                 tr("Split Vertically"));
	settingsManager()->declareAction("paintfield.view.splitHorizontally",
	                                 tr("Split Horizontally"));
	settingsManager()->declareAction("paintfield.view.closeCurrentSplit",
	                                 tr("Close Current Split"));
	
	settingsManager()->declareMenu("paintfield.window",
	                               tr("Window"));
	
	settingsManager()->declareAction("paintfield.window.minimize",
	                                 tr("Minimize"));
	settingsManager()->declareAction("paintfield.window.zoom",
	                                 tr("Zoom"));
	
	settingsManager()->declareMenu("paintfield.help",
	                               tr("Help"));
}

void AppController::createActions()
{
	d->actions << createAction("paintfield.file.quit", d->workspaceManager, SLOT(tryCloseAll()));
	d->actions << createAction("paintfield.window.minimize", this, SLOT(minimizeCurrentWindow()));
	d->actions << createAction("paintfield.window.zoom", this, SLOT(zoomCurrentWindow()));
	d->actions << createAction("paintfield.view.newWorkspace", d->workspaceManager, SLOT(newWorkspace()));
}


AppController *AppController::_instance = 0;


}