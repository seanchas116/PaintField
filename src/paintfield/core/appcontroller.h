#ifndef FSAPPLICATION_H
#define FSAPPLICATION_H

#include <QMainWindow>
#include <QDir>

#include "global.h"

namespace PaintField
{

class Application;
class PaletteManager;
class WorkspaceManager;
class ExtensionManager;
class SettingsManager;
class AppExtension;
class ExtensionFactory;
class CursorStack;
class DocumentReferenceManager;
class Canvas;
class Workspace;

/**
 * AppController is an singleton class that manages application-wide classes.
 */
class AppController : public QObject
{
	Q_OBJECT
public:
	
	explicit AppController(Application *app, QObject *parent = 0);
	~AppController();
	
	void begin();
	
	WorkspaceManager *workspaceManager();
	ExtensionManager *extensionManager();
	SettingsManager *settingsManager();
	CursorStack *cursorStack();
	DocumentReferenceManager *documentReferenceManager();
	
	void addExtensions(const QList<AppExtension *> &extensions);
	QList<AppExtension *> extensions();
	
	void addActions(const QList<QAction *> &actions);
	QList<QAction *> actions();
	
	QString unduplicatedFileTempName(const QString &name);
	
	Application *app();
	
	Canvas *findCanvasWithFilepath(const QString &filepath);
	
	static AppController *instance() { return _instance; }
	
public slots:
	
	void minimizeCurrentWindow();
	void zoomCurrentWindow();
	
	void openFile(const QString &path);
	
	void quit();
	
signals:
	
private:
	
	void declareMenus();
	void createActions();
	
	struct Data;
	Data *d;
	
	static AppController *_instance;
};

/**
 * @return The instance of AppController
 */
inline AppController *appController() { return AppController::instance(); }

}

#endif // FSAPPLICATION_H
