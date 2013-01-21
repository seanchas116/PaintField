#include <QtGui>

#include "debug.h"
#include "util.h"
#include "workspacemanager.h"
#include "modulemanager.h"

#include "appcontroller.h"

namespace PaintField
{

struct AppControllerData
{
	Application *app = 0;
	
	WorkspaceManager *workspaceManager = 0;
	
	QVariant menuBarOrder, workspaceItemOrder;
	
	ToolDeclarationHash toolDeclarationHash;
	ActionDeclarationHash actionDeclarationHash;
	SideBarDeclarationHash sideBarDeclarationHash;
	ToolBarDeclarationHash toolbarInfoHash;
	MenuDeclarationHash menuDeclarationHash;
	
	QHash<QString, QKeySequence> keyBindingHash;
	
	ModuleManager *moduleManager = 0;
	
	QList<AppModule *> modules;
	
	QList<QAction *> actions;
	
	QString lastFileDialogPath;
};

AppController::AppController(Application *app, QObject *parent) :
	QObject(parent),
    d(new AppControllerData)
{
	d->app = app;
	d->workspaceManager = new WorkspaceManager(this);
	d->moduleManager = new ModuleManager(this);
	
	_instance = this;
	
	declareMenus();
	createActions();
	
	// prepare directiries
	
	QDir documentDir(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
	documentDir.mkpath("PaintField/Contents/Brush Presets");
	
	connect(app, SIGNAL(fileOpenRequested(QString)), this, SLOT(openFile(QString)));
}

AppController::~AppController()
{
	delete d;
}

WorkspaceManager *AppController::workspaceManager() { return d->workspaceManager; }

void AppController::loadMenuBarOrderFromJson(const QString &path) { d->menuBarOrder = loadJsonFromFile(path); }
void AppController::loadWorkspaceItemOrderFromJson(const QString &path) { d->workspaceItemOrder = loadJsonFromFile(path); }

void AppController::setMenuBarOrder(const QVariant &order) { d->menuBarOrder = order; }
QVariant AppController::menuBarOrder() const { return d->menuBarOrder; }

void AppController::setWorkspaceItemOrder(const QVariant &order) { d->workspaceItemOrder = order; }

void AppController::declareTool(const QString &name, const ToolDeclaration &info) { d->toolDeclarationHash[name] = info; }
void AppController::declareAction(const QString &name, const ActionDeclaration &info) { d->actionDeclarationHash[name] = info; }
void AppController::declareSideBar(const QString &name, const SidebarDeclaration &info) { d->sideBarDeclarationHash[name] = info; }
void AppController::declareToolbar(const QString &name, const ToolbarDeclaration &info) { d->toolbarInfoHash[name] = info; }
void AppController::declareMenu(const QString &id, const MenuDeclaration &info) { d->menuDeclarationHash[id] = info; }

void AppController::declareTool(const QString &name, const QString &text, const QIcon &icon, const QStringList &supportedLayerTypes)
{ declareTool(name, ToolDeclaration(text, icon, supportedLayerTypes)); }
void AppController::declareAction(const QString &name, const QString &text, const QKeySequence &defaultShortcut)
{ declareAction(name, ActionDeclaration(text, defaultShortcut)); }

ToolDeclarationHash AppController::toolDeclarationHash() const { return d->toolDeclarationHash; }
ActionDeclarationHash AppController::actionDeclarationHash() const { return d->actionDeclarationHash; }
SideBarDeclarationHash AppController::sideBarDeclarationHash() const { return d->sideBarDeclarationHash; }
ToolBarDeclarationHash AppController::toolBarDeclarationHash() const { return d->toolbarInfoHash; }
MenuDeclarationHash AppController::menuDeclarationHash() const { return d->menuDeclarationHash; }

QStringList AppController::toolNames() const { return d->toolDeclarationHash.keys(); }
QStringList AppController::actionNames() const { return d->actionDeclarationHash.keys(); }
QStringList AppController::sidebarNames() const { return d->sideBarDeclarationHash.keys(); }
QStringList AppController::toolbarNames() const { return d->toolbarInfoHash.keys(); }
QStringList AppController::menuNames() const { return d->menuDeclarationHash.keys(); }

QHash<QString, QKeySequence> AppController::keyBindingHash() const { return d->keyBindingHash; }

QList<AppModule *> AppController::modules() { return d->modules; }

void AppController::addActions(const QList<QAction *> &actions) { d->actions += actions; }
QList<QAction *> AppController::actions() { return d->actions; }

QString AppController::lastFileDialogPath() const { return d->lastFileDialogPath; }
void AppController::setLastFileDialogPath(const QString &path) { d->lastFileDialogPath = path; }

Application *AppController::app() { return d->app; }

ModuleManager *AppController::moduleManager()
{
	return d->moduleManager;
}

QVariant AppController::workspaceItemOrder() const
{
	return d->workspaceItemOrder;
}

void AppController::declareMenus()
{
	declareMenu("paintfield.file",
	            tr("File"));
	
	declareAction("paintfield.file.new",
	              tr("New..."));
	
	declareAction("paintfield.file.newFromImageFile",
	              tr("New from Image File..."));
	
	declareAction("paintfield.file.open",
	              tr("Open..."));
	declareAction("paintfield.file.close",
	              tr("Close"));
	declareAction("paintfield.file.save",
	              tr("Save"));
	declareAction("paintfield.file.saveAs",
	              tr("Save As..."));
	declareAction("paintfield.file.export",
	              tr("Export..."));
	declareAction("paintfield.file.quit",
	              tr("Quit"));
	
	declareMenu("paintfield.edit",
	            tr("Edit"));
	
	declareAction("paintfield.edit.undo",
	              tr("Undo"));
	declareAction("paintfield.edit.redo",
	              tr("Redo"));
	declareAction("paintfield.edit.cut",
	              tr("Cut"));
	declareAction("paintfield.edit.copy",
	              tr("Copy"));
	declareAction("paintfield.edit.paste",
	              tr("Paste"));
	declareAction("paintfield.edit.delete",
	              tr("Delete"));
	declareAction("paintfield.edit.selectAll",
	              tr("Select All"));
	
	declareMenu("paintfield.view",
	            tr("View"));
	
	declareAction("paintfield.view.splitVertically",
	              tr("Split Vertically"));
	declareAction("paintfield.view.splitHorizontally",
	              tr("Split Horizontally"));
	declareAction("paintfield.view.closeCurrentSplit",
	              tr("Close Current Split"));
	
	declareMenu("paintfield.window",
	            tr("Window"));
	
	declareAction("paintfield.window.minimize",
	              tr("Minimize"));
	declareAction("paintfield.window.zoom",
	              tr("Zoom"));
	
	declareMenu("paintfield.help",
	            tr("Help"));
}

void AppController::createActions()
{
	d->actions << createAction("paintfield.file.quit", d->workspaceManager, SLOT(tryCloseAll()));
	d->actions << createAction("paintfield.window.minimize", this, SLOT(minimizeCurrentWindow()));
	d->actions << createAction("paintfield.window.zoom", this, SLOT(zoomCurrentWindow()));
}

void AppController::begin()
{
	loadBuiltinSettings();
	loadUserSettings();
	
	moduleManager()->initialize(this);
	addModules(moduleManager()->createAppModules(this, this));
	applyKeyBindingsToActionDeclarations();
	workspaceManager()->newWorkspace();
}

static const QString menuBarOrderFileName("menubar.json");
static const QString WorkspaceItemOrderFileName("workspace-items.json");
static const QString keyBindingFileName("key-bindings.json");


void AppController::loadBuiltinSettings()
{
	QDir dir(builtinDataDir());
	
	if (!dir.exists())
		return;
	
	if (!dir.cd("Settings"))
		return;
	
	loadMenuBarOrderFromJson(dir.filePath(menuBarOrderFileName));
	loadWorkspaceItemOrderFromJson(dir.filePath(WorkspaceItemOrderFileName));
	loadAndAddKeyBindingsFromJson(dir.filePath(keyBindingFileName));
}

void AppController::loadUserSettings()
{
	QDir dir(userDataDir());
	
	if (!dir.exists())
		return;
	
	if (!dir.cd("Settings"))
		return;
	
	QString menubarPath = dir.filePath(menuBarOrderFileName);
	QString workspaceItemPath = dir.filePath(WorkspaceItemOrderFileName);
	QString keymapPath = dir.filePath(keyBindingFileName);
	
	if (QFile::exists(menubarPath))
		loadMenuBarOrderFromJson(menubarPath);
	
	if (QFile::exists(workspaceItemPath))
		loadWorkspaceItemOrderFromJson(workspaceItemPath);
	
	if (QFile::exists(keymapPath))
		loadAndAddKeyBindingsFromJson(keymapPath);
}

void AppController::addModuleFactory(ModuleFactory *factory)
{
	d->moduleManager->addModuleFactory(factory);
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

void AppController::loadAndAddKeyBindingsFromJson(const QString &path)
{
	QVariantMap map = loadJsonFromFile(path).toMap();
	
	for (auto iter = map.begin(); iter != map.end(); ++iter)
	{
		QString id = iter.key();
		QKeySequence key(iter.value().toString());
		
		if (!id.isEmpty() && !key.isEmpty())
			addKeyBinding(id, key);
	}
}

void AppController::addKeyBindingHash(const QHash<QString, QKeySequence> &hash)
{
	for (auto iter = hash.begin(); iter != hash.end(); ++iter)
		addKeyBinding(iter.key(), iter.value());
}

void AppController::addKeyBinding(const QString &name, const QKeySequence &shortcut)
{
	d->keyBindingHash[name] = shortcut;
}

void AppController::applyKeyBindingsToActionDeclarations()
{
	for (auto keyIter = d->keyBindingHash.begin(); keyIter != d->keyBindingHash.end(); ++keyIter)
	{
		for (auto actionIter = d->actionDeclarationHash.begin(); actionIter != d->actionDeclarationHash.end(); ++actionIter)
		{
			if (actionIter.key() == keyIter.key())
				actionIter->shortcut = keyIter.value();
		}
	}
}

void AppController::addModules(const AppModuleList &modules)
{
	for (AppModule *module : modules)
		addActions(module->actions());
	
	d->modules += modules;
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

QString AppController::builtinDataDir() const
{
	return QDir(qApp->applicationDirPath()).absolutePath();
}

QString AppController::userDataDir() const
{
	return QDir(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)).filePath("PaintField");
}

void AppController::openFile(const QString &path)
{
	PAINTFIELD_DEBUG << "file open requested:" << path;
	
	auto workspace = workspaceManager()->currentWorkspace();
	
	if (workspace)
		workspace->openCanvasFromFilepath(path);
}

AppController *AppController::_instance = 0;


}
