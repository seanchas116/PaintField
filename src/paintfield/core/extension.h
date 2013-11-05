#pragma once

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

class Extension : public QObject
{
	Q_OBJECT
public:
	
	explicit Extension(QObject *parent = 0);
	~Extension();
	
	/**
	 * Creates a Tool, which is installed in Canvas and delegates editing.
	 * This function is called when a new canvas is added or the current tool is changed in each canvas.
	 * @param name declared name of the tool
	 * @param canvas
	 * @return 
	 */
	virtual Tool *createTool(const QString &name, Canvas *canvas);
	
	/**
	 * Updates a toolbar.
	 * This function is called when a new canvas is added or the current canvas is changed in a workspace.
	 * @param toolBar
	 * @param name
	 */
	virtual void updateToolBar(QToolBar *toolBar, const QString &name);
	
	QActionList actions();
	
	/**
	 * Adds an action which is used in the menubar.
	 * @param action
	 */
	void addAction(QAction *action);
	
	void addActions(const QList<QAction *> &actions);
	
	QHash<QString, QWidget *> sideBars();
	
	/**
	 * Adds an side bar which is used in the workspace view.
	 * @param name
	 * @return 
	 */
	QWidget *sideBar(const QString &id);
	void addSideBar(const QString &id, QWidget *sideBar);
	
private:
	
	struct Data;
	Data *d;
};

class CanvasExtension : public Extension
{
	Q_OBJECT
public:
	
	CanvasExtension(Canvas *canvas, QObject *parent) : Extension(parent), _canvas(canvas) {}
	
	Canvas *canvas() { return _canvas; }
	
private:
	
	Canvas *_canvas = 0;
};
typedef QList<CanvasExtension *> CanvasExtensionList;

class WorkspaceExtension : public Extension
{
	Q_OBJECT
public:
	
	WorkspaceExtension(Workspace *workspace, QObject *parent) : Extension(parent), _workspace(workspace) {}
	
	Workspace *workspace() { return _workspace; }
	
private:
	
	Workspace *_workspace;
};
typedef QList<WorkspaceExtension *> WorkspaceExtensionList;

class AppExtension : public Extension
{
	Q_OBJECT
public:
	
	explicit AppExtension(AppController *app, QObject *parent) : Extension(parent), _app(app) {}
	AppController *app() { return _app; }
	
private:
	
	AppController *_app;
};
typedef QList<AppExtension *> AppExtensionList;

namespace ExtensionUtil
{

Tool *createTool(const AppExtensionList &appExtensions, const WorkspaceExtensionList &workspaceExtensions, const CanvasExtensionList &canvasModules, const QString &name, Canvas *canvas);
QWidget *sideBarForWorkspace(const AppExtensionList &appExtensions, const WorkspaceExtensionList &workspaceExtensions, const QString &name);
QWidget *sideBarForCanvas(const CanvasExtensionList &canvasExtensions, const QString &name);
void updateToolBar(const AppExtensionList &appExtensions, const WorkspaceExtensionList &workspaceExtensions, const CanvasExtensionList &canvasExtensions, QToolBar *toolBar, const QString &name);

}

class ExtensionFactory : public QObject
{
	Q_OBJECT
	
public:
	
	explicit ExtensionFactory(QObject *parent = 0) : QObject(parent) {}
	
	virtual void initialize(AppController *app) = 0;
	
	virtual AppExtensionList createAppExtensions(AppController *app, QObject *parent);
	virtual WorkspaceExtensionList createWorkspaceExtensions(Workspace *workspace, QObject *parent);
	virtual CanvasExtensionList createCanvasExtensions(Canvas *canvas, QObject *parent);
	
	QList<ExtensionFactory *> subExtensionFactories() { return _subExtensionFactories; }
	void addSubExtensionFactory(ExtensionFactory *factory);
	
private:
	
	QList<ExtensionFactory *> _subExtensionFactories;
};

}

