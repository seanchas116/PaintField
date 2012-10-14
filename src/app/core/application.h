#ifndef FSAPPLICATION_H
#define FSAPPLICATION_H

#include <QMainWindow>
#include "tabletapplication.h"

#include "sidebarfactory.h"
#include "tool.h"
#include "palettemanager.h"
#include "workspacemanager.h"

namespace PaintField {

QVariant loadJsonFromFile(const QString &path);

class Application : public TabletApplication
{
	Q_OBJECT
public:
	
	Application(int &argv, char **args);
	
	/**
	 * @return The workspace manager
	 */
	WorkspaceManager *workspaceManager() { return _workspaceManager; }
	
	/**
	 * @return The registered tool factories
	 */
	QList<ToolFactory *> toolFactories() { return _toolFactories; }
	
	/**
	 * Registers a tool factory.
	 * @param factory The tool factory
	 */
	void addToolFactory(ToolFactory *factory);
	
	QList<SidebarFactory *> sidebarFactories() { return _sidebarFactories; }
	
	void addSidebarFactory(SidebarFactory *factory);
	
	
	/**
	 * Declares the title of a menu item (menu / action).
	 * @param id The item's object name
	 * @param title
	 */
	void declareMenuItemTitle(const QString &id, const QString &title) { _menuItemTitles.insert(id, title); }
	
	/**
	 * Declares a key map for a action ID.
	 * @param id The action's object name
	 * @param shortcut
	 */
	void declareActionKeyMap(const QString &id, const QKeySequence &shortcut) { _actionKeyMaps.insert(id, shortcut); }
	
	/**
	 * @return The declared menu item titles
	 */
	QHash<QString, QString> menuItemTitles() const{ return _menuItemTitles; }
	
	/**
	 * @return The declared action key bindings
	 */
	QHash<QString, QKeySequence> actionShortcuts() const { return _actionKeyMaps; }
	
	void loadMenuBarOrderFromJson(const QString &path) { _menuBarOrder = loadJsonFromFile(path); }
	void loadPanelOrderFromJson(const QString &path) { _panelOrder = loadJsonFromFile(path); }
	void loadKeyMapFromJson(const QString &path);
	
	void setMenuBarOrder(const QVariant &order) { _menuBarOrder = order; }
	QVariant menuBarOrder() const { return _menuBarOrder; }
	
	void setPanelOrder(const QVariant &order) { _panelOrder = order; }
	QVariant panelOrder() const { return _panelOrder; }
	
	ActionManager *actionManager() { return _actionManager; }
	
signals:
	
public slots:
	
	void tryQuit();
	void minimizeCurrentWindow();
	void zoomCurrentWindow();
	
private:
	
	WorkspaceManager *_workspaceManager;
	
	QList<ToolFactory *> _toolFactories;
	QList<SidebarFactory *> _sidebarFactories;
	
	QHash<QString, QString> _menuItemTitles;
	QHash<QString, QKeySequence> _actionKeyMaps;
	
	QVariant _menuBarOrder, _panelOrder;
	
	ActionManager *_actionManager;
};

/**
 * @return The instance of Application
 */
inline Application *app() { return static_cast<Application *>(QCoreApplication::instance()); }

}

#endif // FSAPPLICATION_H
