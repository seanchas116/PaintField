#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include <QIcon>
#include <QKeySequence>
#include <QHash>

class QAction;
class QToolBar;

typedef QList<QAction *> QActionList;

namespace PaintField
{

class AppController;
class Workspace;
class Canvas;
class CanvasView;
class Tool;

class Module : public QObject
{
	Q_OBJECT
public:
	
	explicit Module(QObject *parent = 0) : QObject(parent) {}
	
	~Module();
	
	/**
	 * Creates a Tool, which is installed in CanvasView and delegates editing.
	 * This function is called when a new canvas is added or the current tool is changed for each canvas.
	 * @param name declared name of the tool
	 * @param canvas
	 * @return 
	 */
	virtual Tool *createTool(const QString &name, Canvas *canvas);
	
	/**
	 * Updates a toolbar.
	 * @param toolBar
	 * @param name
	 */
	virtual void updateToolBar(QToolBar *toolBar, const QString &name);
	
	QActionList actions() { return _actions; }
	
	void addAction(QAction *action) { _actions << action; }
	
	QHash<QString, QWidget *> sideBars() { return _sideBars; }
	QWidget *sideBar(const QString &name) { return _sideBars.value(name, 0); }
	void addSideBar(const QString &name, QWidget *sideBar);
	
private:
	
	QActionList _actions;
	QHash<QString, QWidget *> _sideBars;
};

class CanvasModule : public Module
{
	Q_OBJECT
public:
	
	CanvasModule(Canvas *canvas, QObject *parent) : Module(parent), _canvas(canvas) {}
	
	Canvas *canvas() { return _canvas; }
	
private:
	
	Canvas *_canvas = 0;
};
typedef QList<CanvasModule *> CanvasModuleList;

class WorkspaceModule : public Module
{
	Q_OBJECT
public:
	
	WorkspaceModule(Workspace *workspace, QObject *parent) : Module(parent), _workspace(workspace) {}
	
	Workspace *workspace() { return _workspace; }
	
private:
	
	Workspace *_workspace;
};
typedef QList<WorkspaceModule *> WorkspaceModuleList;

class AppModule : public Module
{
	Q_OBJECT
public:
	
	explicit AppModule(AppController *app, QObject *parent) : Module(parent), _app(app) {}
	AppController *app() { return _app; }
	
private:
	
	AppController *_app;
};
typedef QList<AppModule *> AppModuleList;

Tool *createTool(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const CanvasModuleList &canvasModules, const QString &name, Canvas *canvas);
QWidget *sideBarForWorkspace(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const QString &name);
QWidget *sideBarForCanvas(const CanvasModuleList &canvasModules, const QString &name);
void updateToolBar(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const CanvasModuleList &canvasModules, QToolBar *toolBar, const QString &name);

class ModuleFactory : public QObject
{
	Q_OBJECT
	
public:
	
	explicit ModuleFactory(QObject *parent = 0) : QObject(parent) {}
	
	virtual void initialize(AppController *app) = 0;
	
	virtual AppModuleList createAppModules(AppController *app, QObject *parent);
	virtual WorkspaceModuleList createWorkspaceModules(Workspace *workspace, QObject *parent);
	virtual CanvasModuleList createCanvasModules(Canvas *canvas, QObject *parent);
	
	QList<ModuleFactory *> subModuleFactories() { return _subModuleFactories; }
	void addSubModuleFactory(ModuleFactory *factory) { _subModuleFactories << factory; }
	
private:
	
	QList<ModuleFactory *> _subModuleFactories;
};

}

#endif // MODULE_H
