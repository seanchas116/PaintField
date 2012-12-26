#ifndef FSAPPLICATION_H
#define FSAPPLICATION_H

#include <QMainWindow>
#include <QDir>

#include "application.h"
#include "util.h"
#include "palettemanager.h"
#include "workspacemanager.h"
#include "global.h"

namespace PaintField
{

class ModuleManager;
class AppModule;
class ModuleFactory;

class AppController : public QObject
{
	Q_OBJECT
public:
	
	explicit AppController(Application *app, QObject *parent = 0);
	
	void begin();
	
	/**
	 * @return The workspace manager
	 */
	WorkspaceManager *workspaceManager() { return _workspaceManager; }
	
	void loadMenuBarOrderFromJson(const QString &path) { _menuBarOrder = loadJsonFromFile(path); }
	void loadWorkspaceItemOrderFromJson(const QString &path) { _workspaceItemOrder = loadJsonFromFile(path); }
	void loadKeyMapFromJson(const QString &path);
	
	void setMenuBarOrder(const QVariant &order) { _menuBarOrder = order; }
	QVariant menuBarOrder() const { return _menuBarOrder; }
	
	void setWorkspaceItemOrder(const QVariant &order) { _workspaceItemOrder = order; }
	QVariant workspaceItemOrder() const { return _workspaceItemOrder; }
	
	ModuleManager *moduleManager() { return _moduleManager; }
	void addModuleFactory(ModuleFactory *factory);
	
	void declareTool(const QString &name, const ToolDeclaration &info) { _toolDeclarationHash[name] = info; }
	void declareAction(const QString &name, const ActionDeclaration &info) { _actionDeclarationHash[name] = info; }
	void declareSideBar(const QString &name, const SidebarDeclaration &info) { _sideBarDeclarationHash[name] = info; }
	void declareToolbar(const QString &name, const ToolbarDeclaration &info) { _toolbarInfoHash[name] = info; }
	void declareMenu(const QString &id, const MenuDeclaration &info) { _menuDeclarationHash[id] = info; }
	
	void declareTool(const QString &name, const QString &text, const QIcon &icon, const QStringList &supportedLayerTypes) { declareTool(name, ToolDeclaration(text, icon, supportedLayerTypes)); }
	void declareAction(const QString &name, const QString &text, const QKeySequence &defaultShortcut = QKeySequence()) { declareAction(name, ActionDeclaration(text, defaultShortcut)); }
	
	ToolDeclarationHash toolDeclarationHash() const { return _toolDeclarationHash; }
	ActionDeclarationHash actionDeclarationHash() const { return _actionDeclarationHash; }
	SideBarDeclarationHash sideBarDeclarationHash() const { return _sideBarDeclarationHash; }
	ToolBarDeclarationHash toolBarDeclarationHash() const { return _toolbarInfoHash; }
	MenuDeclarationHash menuDeclarationHash() const { return _menuDeclarationHash; }
	
	QStringList toolNames() const { return _toolDeclarationHash.keys(); }
	QStringList actionNames() const { return _actionDeclarationHash.keys(); }
	QStringList sidebarNames() const { return _sideBarDeclarationHash.keys(); }
	QStringList toolbarNames() const { return _toolbarInfoHash.keys(); }
	QStringList menuNames() const { return _menuDeclarationHash.keys(); }
	
	void overrideActionShortcut(const QString &name, const QKeySequence &shortcut);
	
	void addModules(const QList<AppModule *> &modules);
	QList<AppModule *> modules() { return _modules; }
	
	void addActions(const QList<QAction *> &actions) { _actions += actions; }
	QList<QAction *> actions() { return _actions; }
	
	QString unduplicatedTempName(const QString &name);
	
	QString builtinContentsDir() const;
	QString userContentsDir() const;
	
	Application *app() { return _app; }
	
	static AppController *instance() { return _instance; }
	
public slots:
	
	void handleMessage(const QString &message);
	
	void minimizeCurrentWindow();
	void zoomCurrentWindow();
	
	void openFile(const QString &path);
	
signals:
	
private:
	
	void declareMenus();
	void createActions();
	
	Application *_app = 0;
	
	WorkspaceManager *_workspaceManager = 0;
	
	QVariant _menuBarOrder, _workspaceItemOrder;
	
	ToolDeclarationHash _toolDeclarationHash;
	ActionDeclarationHash _actionDeclarationHash;
	SideBarDeclarationHash _sideBarDeclarationHash;
	ToolBarDeclarationHash _toolbarInfoHash;
	MenuDeclarationHash _menuDeclarationHash;
	
	ModuleManager *_moduleManager = 0;
	
	QList<AppModule *> _modules;
	
	QList<QAction *> _actions;
	
	static AppController *_instance;
};

/**
 * @return The instance of AppController
 */
inline AppController *appController() { return AppController::instance(); }

}

#endif // FSAPPLICATION_H
