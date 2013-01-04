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

struct AppControllerData;

class AppController : public QObject
{
	Q_OBJECT
public:
	
	explicit AppController(Application *app, QObject *parent = 0);
	~AppController();
	
	void begin();
	
	/**
	 * @return The workspace manager
	 */
	WorkspaceManager *workspaceManager();
	
	void loadBuiltinSettings();
	void loadUserSettings();
	
	void loadMenuBarOrderFromJson(const QString &path);
	void loadWorkspaceItemOrderFromJson(const QString &path);
	void loadAndAddKeyBindingsFromJson(const QString &path);
	
	void setMenuBarOrder(const QVariant &order);
	QVariant menuBarOrder() const;
	
	void setWorkspaceItemOrder(const QVariant &order);
	QVariant workspaceItemOrder() const;
	
	ModuleManager *moduleManager();
	void addModuleFactory(ModuleFactory *factory);
	
	void declareTool(const QString &name, const ToolDeclaration &info);
	void declareAction(const QString &name, const ActionDeclaration &info);
	void declareSideBar(const QString &name, const SidebarDeclaration &info);
	void declareToolbar(const QString &name, const ToolbarDeclaration &info);
	void declareMenu(const QString &id, const MenuDeclaration &info);
	
	void declareTool(const QString &name, const QString &text, const QIcon &icon, const QStringList &supportedLayerTypes);
	void declareAction(const QString &name, const QString &text, const QKeySequence &defaultShortcut = QKeySequence());
	
	ToolDeclarationHash toolDeclarationHash() const;
	ActionDeclarationHash actionDeclarationHash() const;
	SideBarDeclarationHash sideBarDeclarationHash() const;
	ToolBarDeclarationHash toolBarDeclarationHash() const;
	MenuDeclarationHash menuDeclarationHash() const;
	
	QStringList toolNames() const;
	QStringList actionNames() const;
	QStringList sidebarNames() const;
	QStringList toolbarNames() const;
	QStringList menuNames() const;
	
	QHash<QString, QKeySequence> keyBindingHash() const;
	void addKeyBindingHash(const QHash<QString, QKeySequence> &hash);
	void addKeyBinding(const QString &name, const QKeySequence &shortcut);
	
	void addModules(const QList<AppModule *> &modules);
	QList<AppModule *> modules();
	
	void addActions(const QList<QAction *> &actions);
	QList<QAction *> actions();
	
	QString unduplicatedNewFileTempName();
	QString unduplicatedTempName(const QString &name);
	
	QString builtinDataDir() const;
	QString userDataDir() const;
	
	QString lastFileDialogPath() const;
	void setLastFileDialogPath(const QString &path);
	
	Application *app();
	
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
	void applyKeyBindingsToActionDeclarations();
	
	AppControllerData *d;
	
	static AppController *_instance;
};

/**
 * @return The instance of AppController
 */
inline AppController *appController() { return AppController::instance(); }

}

#endif // FSAPPLICATION_H
