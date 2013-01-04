#include <QtGui>

#include "debug.h"
#include "util.h"
#include "workspacemanager.h"
#include "modulemanager.h"

#include "appcontroller.h"

namespace PaintField
{

AppController::AppController(Application *app, QObject *parent) :
	QObject(parent),
    _app(app),
    _workspaceManager(new WorkspaceManager(this)),
    _moduleManager(new ModuleManager(this))
{
	_instance = this;
	
	declareMenus();
	createActions();
	
	// prepare directiries
	
	QDir documentDir(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
	documentDir.mkpath("PaintField/Contents/Brush Presets");
	
	connect(app, SIGNAL(fileOpenRequested(QString)), this, SLOT(openFile(QString)));
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
	_actions << createAction("paintfield.file.quit", _workspaceManager, SLOT(tryCloseAll()));
	_actions << createAction("paintfield.window.minimize", this, SLOT(minimizeCurrentWindow()));
	_actions << createAction("paintfield.window.zoom", this, SLOT(zoomCurrentWindow()));
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
	_moduleManager->addModuleFactory(factory);
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
	_keyBindingHash[name] = shortcut;
}

void AppController::applyKeyBindingsToActionDeclarations()
{
	for (auto keyIter = _keyBindingHash.begin(); keyIter != _keyBindingHash.end(); ++keyIter)
	{
		for (auto actionIter = _actionDeclarationHash.begin(); actionIter != _actionDeclarationHash.end(); ++actionIter)
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
	
	_modules += modules;
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
		workspace->addCanvasFromFile(path);
}

AppController *AppController::_instance = 0;


}
