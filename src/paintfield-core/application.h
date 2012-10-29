#ifndef FSAPPLICATION_H
#define FSAPPLICATION_H

#include <QMainWindow>
#include "tabletapplication.h"

#include "util.h"
#include "palettemanager.h"
#include "workspacemanager.h"

namespace PaintField
{

class ModuleManager;
class AppModule;
class ModuleFactory;

struct ToolInfo
{
	ToolInfo() {}
	ToolInfo(const QString &text, const QIcon &icon, const QVector<int> &supportedLayerTypes) : text(text), icon(icon), supportedLayerTypes(supportedLayerTypes) {}
	QString text;
	QIcon icon;
	QVector<int> supportedLayerTypes;
};
typedef QHash<QString, ToolInfo> ToolInfoHash;

struct ActionInfo
{
	ActionInfo() {}
	ActionInfo(const QString &text, const QKeySequence &defaultShortcut = QKeySequence()) : text(text), shortcut(defaultShortcut) {}
	QString text;
	QKeySequence shortcut;
};
typedef QHash<QString, ActionInfo> ActionInfoHash;

struct SidebarInfo
{
	SidebarInfo() {}
	SidebarInfo(const QString &text) : text(text) {}
	QString text;
};
typedef QHash<QString, SidebarInfo> SidebarInfoHash;

struct ToolbarInfo
{
	ToolbarInfo() {}
	ToolbarInfo(const QString &text) : text(text) {}
	QString text;
};
typedef QHash<QString, ToolbarInfo> ToolbarInfoHash;

class Application : public TabletApplication
{
	Q_OBJECT
public:
	
	Application(int &argv, char **args);
	
	int exec();
	
	/**
	 * @return The workspace manager
	 */
	WorkspaceManager *workspaceManager() { return _workspaceManager; }
	
	void loadMenuBarOrderFromJson(const QString &path) { _menuBarOrder = loadJsonFromFile(path); }
	void loadPanelOrderFromJson(const QString &path) { _sidebarOrder = loadJsonFromFile(path); }
	void loadKeyMapFromJson(const QString &path);
	
	void setMenuBarOrder(const QVariant &order) { _menuBarOrder = order; }
	QVariant menuBarOrder() const { return _menuBarOrder; }
	
	void setSidebarOrder(const QVariant &order) { _sidebarOrder = order; }
	QVariant sidebarOrder() const { return _sidebarOrder; }
	
	ModuleManager *moduleManager() { return _moduleManager; }
	void addModuleFactory(ModuleFactory *factory);
	
	void declareTool(const QString &name, const ToolInfo &info) { _toolInfoHash[name] = info; }
	void declareAction(const QString &name, const ActionInfo &info) { _actionInfoHash[name] = info; }
	void declareSidebar(const QString &name, const SidebarInfo &info) { _sidebarInfoHash[name] = info; }
	void declareToolbar(const QString &name, const ToolbarInfo &info) { _toolbarInfoHash[name] = info; }
	
	ToolInfoHash toolInfoHash() const { return _toolInfoHash; }
	ActionInfoHash actionInfoHash() const { return _actionInfoHash; }
	SidebarInfoHash sidebarInfoHash() const { return _sidebarInfoHash; }
	ToolbarInfoHash toolBarInfoHash() const { return _toolbarInfoHash; }
	
	QStringList toolNames() const { return _toolInfoHash.keys(); }
	QStringList actionNames() const { return _actionInfoHash.keys(); }
	QStringList sidebarNames() const { return _sidebarInfoHash.keys(); }
	QStringList toolbarNames() const { return _toolbarInfoHash.keys(); }
	
	void declareMenu(const QString &id, const QString &text) { _menuHash[id] = text; }
	QHash<QString, QString> menuHash() const { return _menuHash; }
	
	void overrideActionShortcut(const QString &name, const QKeySequence &shortcut);
	
	void addModules(const QList<AppModule *> &modules);
	QList<AppModule *> modules() { return _modules; }
	
	void addActions(const QList<QAction *> &actions) { _actions += actions; }
	QList<QAction *> actions() { return _actions; }
	
signals:
	
public slots:
	
	void tryQuit();
	void minimizeCurrentWindow();
	void zoomCurrentWindow();
	
private:
	
	WorkspaceManager *_workspaceManager = 0;
	
	QVariant _menuBarOrder, _sidebarOrder;
	QHash<QString, QString> _menuHash;
	
	ToolInfoHash _toolInfoHash;
	ActionInfoHash _actionInfoHash;
	SidebarInfoHash _sidebarInfoHash;
	ToolbarInfoHash _toolbarInfoHash;
	
	ModuleManager *_moduleManager = 0;
	
	QList<AppModule *> _modules;
	
	QList<QAction *> _actions;
};

/**
 * @return The instance of Application
 */
inline Application *app() { return static_cast<Application *>(QCoreApplication::instance()); }

}

#endif // FSAPPLICATION_H
