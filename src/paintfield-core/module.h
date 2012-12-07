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
class WorkspaceController;
class CanvasController;
class CanvasView;
class Tool;

class Module : public QObject
{
	Q_OBJECT
public:
	
	explicit Module(QObject *parent = 0) : QObject(parent) {}
	
	/**
	 * Creates a Tool, which is installed in CanvasView and delegates editing.
	 * This function is called when a new canvas is added or the current tool is changed for each canvas.
	 * @param name declared name of the tool
	 * @param view
	 * @return 
	 */
	virtual Tool *createTool(const QString &name, CanvasView *view);
	
	/**
	 * Creates a sidebar.
	 * If "this" is a canvas module, this function is called when the current canvas is changed.
	 * Otherwise, it is called when a new workspace is added.
	 * @param name
	 * @return 
	 */
	virtual QWidget *createSideBar(const QString &name);
	
	/**
	 * Updates a toolbar.
	 * @param toolBar
	 * @param name
	 */
	virtual void updateToolBar(QToolBar *toolBar, const QString &name);
	
	QActionList actions() { return _actions; }
	
	void addAction(QAction *action) { _actions << action; }
	
private:
	
	QActionList _actions;
};

class CanvasModule : public Module
{
	Q_OBJECT
public:
	
	CanvasModule(CanvasController *canvas, QObject *parent) : Module(parent), _canvas(canvas) {}
	
	CanvasController *canvas() { return _canvas; }
	
private:
	
	CanvasController *_canvas = 0;
};
typedef QList<CanvasModule *> CanvasModuleList;

class WorkspaceModule : public Module
{
	Q_OBJECT
public:
	
	WorkspaceModule(WorkspaceController *workspace, QObject *parent) : Module(parent), _workspace(workspace) {}
	
	WorkspaceController *workspace() { return _workspace; }
	
private:
	
	WorkspaceController *_workspace;
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

Tool *createTool(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const CanvasModuleList &canvasModules, const QString &name, CanvasView *view);
QWidget *createSideBarForWorkspace(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const QString &name);
QWidget *createSideBarForCanvas(const CanvasModuleList &canvasModules, const QString &name);
void updateToolBar(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const CanvasModuleList &canvasModules, QToolBar *toolBar, const QString &name);

class ModuleFactory : public QObject
{
	Q_OBJECT
	
public:
	
	explicit ModuleFactory(QObject *parent = 0) : QObject(parent) {}
	
	virtual void initialize(AppController *app) = 0;
	
	virtual AppModuleList createAppModules(AppController *app, QObject *parent);
	virtual WorkspaceModuleList createWorkspaceModules(WorkspaceController *workspace, QObject *parent);
	virtual CanvasModuleList createCanvasModules(CanvasController *canvas, QObject *parent);
	
	QList<ModuleFactory *> subModuleFactories() { return _subModuleFactories; }
	void addSubModuleFactory(ModuleFactory *factory) { _subModuleFactories << factory; }
	
private:
	
	QList<ModuleFactory *> _subModuleFactories;
};

}

#endif // MODULE_H
