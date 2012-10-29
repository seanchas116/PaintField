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

class Application;
class WorkspaceController;
class CanvasController;
class CanvasView;
class Tool;

class Module : public QObject
{
	Q_OBJECT
public:
	
	explicit Module(QObject *parent = 0) : QObject(parent) {}
	
	virtual Tool *createTool(const QString &name, CanvasView *view);
	virtual QWidget *createSidebar(const QString &name);
	virtual QToolBar *createToolbar(const QString &name);
	
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
	
	explicit AppModule(Application *app, QObject *parent) : Module(parent), _app(app) {}
	Application *app() { return _app; }
	
private:
	
	Application *_app;
};
typedef QList<AppModule *> AppModuleList;

Tool *createTool(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const CanvasModuleList &canvasModules, const QString &name, CanvasView *view);
QWidget *createSidebarForWorkspace(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const QString &name);
QWidget *createSidebarForCanvas(const CanvasModuleList &canvasModules, const QString &name);

class ModuleFactory
{
public:
	
	virtual void initialize(Application *app) = 0;
	
	virtual AppModuleList createAppModules(Application *app, QObject *parent);
	virtual WorkspaceModuleList createWorkspaceModules(WorkspaceController *workspace, QObject *parent);
	virtual CanvasModuleList createCanvasModules(CanvasController *canvas, QObject *parent);
	
	QList<ModuleFactory *> subModuleFactories() { return _subModuleFactories; }
	void addSubModuleFactory(ModuleFactory *factory) { _subModuleFactories << factory; }
	
private:
	
	QList<ModuleFactory *> _subModuleFactories;
};

}

#endif // MODULE_H
