#include <QtGui>

#include "util.h"
#include "workspacemanager.h"
#include "modulemanager.h"

#include "appcontroller.h"

namespace PaintField
{

AppController::AppController(QObject *parent) :
	QObject(parent)
{
	_instance = this;
	
	_workspaceManager = new WorkspaceManager(this);
	_moduleManager = new ModuleManager(this);
	
	declareMenu("paintfield.file", tr("File"));
	
	declareAction("paintfield.file.new", tr("New..."));
	declareAction("paintfield.file.open", tr("Open..."));
	declareAction("paintfield.file.close", tr("Close"));
	declareAction("paintfield.file.save", tr("Save"));
	declareAction("paintfield.file.saveAs", tr("Save As..."));
	declareAction("paintfield.file.export", tr("Export..."));
	declareAction("paintfield.file.quit", tr("Quit"));
	
	declareMenu("paintfield.edit", tr("Edit"));
	
	declareAction("paintfield.edit.undo", tr("Undo"));
	declareAction("paintfield.edit.redo", tr("Redo"));
	declareAction("paintfield.edit.cut", tr("Cut"));
	declareAction("paintfield.edit.copy", tr("Copy"));
	declareAction("paintfield.edit.paste", tr("Paste"));
	declareAction("paintfield.edit.delete", tr("Delete"));
	declareAction("paintfield.edit.selectAll", tr("Select All"));
	
	declareMenu("paintfield.window", tr("Window"));
	
	declareAction("paintfield.window.minimize", tr("Minimize"));
	declareAction("paintfield.window.zoom", tr("Zoom"));
	
	declareMenu("paintfield.help", tr("Help"));
	
	_actions << createAction("paintfield.file.quit", _workspaceManager, SLOT(tryCloseAll()));
	_actions << createAction("paintfield.window.minimize", this, SLOT(minimizeCurrentWindow()));
	_actions << createAction("paintfield.window.zoom", this, SLOT(zoomCurrentWindow()));
}

void AppController::begin()
{
	moduleManager()->initialize(this);
	addModules(moduleManager()->createAppModules(this, this));
	workspaceManager()->newWorkspace();
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

void AppController::loadKeyMapFromJson(const QString &path)
{
	QVariantMap map = loadJsonFromFile(path).toMap();
	
	for (auto iter = map.begin(); iter != map.end(); ++iter)
	{
		QString id = iter.key();
		QKeySequence key(iter.value().toString());
		if (!id.isEmpty() && !key.isEmpty())
		{
			overrideActionShortcut(id, key);
		}
	}
}

void AppController::overrideActionShortcut(const QString &name, const QKeySequence &shortcut)
{
	for (auto iter = _actionDeclarationHash.begin(); iter != _actionDeclarationHash.end(); ++iter)
		if (iter.key() == name)
			iter->shortcut = shortcut;
}

void AppController::addModules(const AppModuleList &modules)
{
	for (AppModule *module : modules)
		addActions(module->actions());
	
	_modules += modules;
}

AppController *AppController::_instance = 0;


}
