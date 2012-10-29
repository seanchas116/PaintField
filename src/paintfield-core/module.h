#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include <QIcon>
#include <QKeySequence>
#include <QHash>

#include "application.h"
#include "workspacecontroller.h"
#include "canvascontroller.h"

class QAction;

typedef QList<QAction *> QActionList;

namespace PaintField
{

class Tool;

class Module : public QObject
{
	Q_OBJECT
public:
	
	explicit Module(QObject *parent = 0) : QObject(parent) {}
	
	virtual Tool *createTool(const QString &name, CanvasView *view) { Q_UNUSED(name) Q_UNUSED(view) return 0; }
	virtual QWidget *createSidebar(const QString &name) { Q_UNUSED(name) return 0; }
	virtual QToolBar *createToolbar(const QString &name) { Q_UNUSED(name) return 0; }
	
	QActionList actions() { return _actions; }
	
	void addAction(QAction *action) { _actions << action; }
	
private:
	
	QActionList _actions;
};

class CanvasModule : public Module
{
	Q_OBJECT
public:
	
	explicit CanvasModule(CanvasController *parent) : Module(parent) {}
	CanvasController *canvas() { return static_cast<CanvasController *>(parent()); }
};
typedef QList<CanvasModule *> CanvasModuleList;

class WorkspaceModule : public Module
{
	Q_OBJECT
public:
	
	explicit WorkspaceModule(WorkspaceController *parent) : Module(parent) {}
	WorkspaceController *workspace() { return static_cast<WorkspaceController *>(parent()); }
};
typedef QList<WorkspaceModule *> WorkspaceModuleList;

class AppModule : public Module
{
	Q_OBJECT
public:
	
	explicit AppModule(Application *parent) : Module(parent) {}
	Application *app() { return static_cast<Application *>(parent()); }
};
typedef QList<AppModule *> AppModuleList;

Tool *createTool(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const CanvasModuleList &canvasModules, const QString &name, CanvasView *view);
QWidget *createSidebarForWorkspace(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const QString &name);
QWidget *createSidebarForCanvas(const CanvasModuleList &canvasModules, const QString &name);


class ModuleFactory
{
public:
	
	virtual void initialize(Application *app) = 0;
	
	virtual QList<AppModule *> createAppModules(Application *app) { Q_UNUSED(app) return QList<AppModule *>(); }
	virtual QList<WorkspaceModule *> createWorkspaceModules(WorkspaceController *workspace) { Q_UNUSED(workspace) return QList<WorkspaceModule *>(); }
	virtual QList<CanvasModule *> createCanvasModules(CanvasController *canvas) {Q_UNUSED(canvas) return QList<CanvasModule *>(); }
	
	QList<ModuleFactory *> subModuleFactories() { return _subModuleFactories; }
	void addSubModuleFactory(ModuleFactory *factory) { _subModuleFactories << factory; }
	
private:
	
	QList<ModuleFactory *> _subModuleFactories;
};

}

#endif // MODULE_H
