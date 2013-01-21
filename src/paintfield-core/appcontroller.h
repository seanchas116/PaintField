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
class SettingsManager;
class AppModule;
class ModuleFactory;

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
	
	ModuleManager *moduleManager();
	
	SettingsManager *settingsManager();
	
	void addModuleFactory(ModuleFactory *factory);
	
	void addModules(const QList<AppModule *> &modules);
	QList<AppModule *> modules();
	
	void addActions(const QList<QAction *> &actions);
	QList<QAction *> actions();
	
	QString unduplicatedNewFileTempName();
	QString unduplicatedTempName(const QString &name);
	
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
	
	class Data;
	Data *d;
	
	static AppController *_instance;
};

/**
 * @return The instance of AppController
 */
inline AppController *appController() { return AppController::instance(); }

}

#endif // FSAPPLICATION_H
