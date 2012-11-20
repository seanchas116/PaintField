#ifndef WORKSPACECONTROLLER_H
#define WORKSPACECONTROLLER_H

#include <QObject>
#include <QPointer>

#include "canvascontroller.h"
#include "workspaceview.h"

namespace PaintField
{

class WorkspaceModule;
class ToolManager;
class PaletteManager;
class WorkspaceCanvasAreaController;

class WorkspaceController : public QObject
{
	Q_OBJECT
public:
	explicit WorkspaceController(QObject *parent = 0);
	
	ToolManager *toolManager() { return _toolManager; }
	PaletteManager *paletteManager() { return _paletteManager; }
	
	WorkspaceView *view() { return _view.data(); }
	void updateView();
	
	void addModules(const WorkspaceModuleList &modules);
	WorkspaceModuleList modules() { return _modules; }
	
	void addActions(const QActionList &actions) { _actions += actions; }
	QActionList actions() { return _actions; }
	
	void addNullCanvasModules(const CanvasModuleList &modules);
	CanvasModuleList nullCanvasModules() { return _nullCanvasModules; }
	
	void addNullCanvasActions(const QActionList &actions) { _nullCanvasActions += actions; }
	QActionList nullCanvasActions() { return _nullCanvasActions; }
	
	void addCanvas(CanvasController *canvas);
	
	QList<CanvasController *> canvases() { return _canvasControllers; }
	
signals:
	
	void currentCanvasChanged(CanvasController *canvas);
	
	void canvasAdded(CanvasController *canvas);
	
	void canvasAboutToBeRemoved(CanvasController *canvas);
	void focused();
	
	void shouldBeDeleted(WorkspaceController *workspace);
	
public slots:
	
	void setFocus();
	
	/**
	 * Sets the current canvas.
	 * @param canvas
	 */
	void setCurrentCanvas(CanvasController *canvas);
	
	/**
	 * Creates a new document and adds a canvas of it.
	 */
	void newCanvas();
	
	/**
	 * Opens a document and adds a canvas of it.
	 */
	void openCanvas();
	
	/**
	 * Try to close all canvases.
	 * @return If succeeded
	 */
	bool tryClose();
	
protected:
	
	bool eventFilter(QObject *watched, QEvent *event);
	
private slots:
	
	void removeCanvas(CanvasController *canvas);
	
private:
	
	QActionList currentCanvasActions() { return _currentCanvas ? _currentCanvas->actions() : _nullCanvasActions; }
	CanvasModuleList currentCanvasModules() { return _currentCanvas ? _currentCanvas->modules() : _nullCanvasModules; }
	
	void updateWorkspaceItems();
	void updateWorkspaceItemsForCanvas(CanvasController *canvas);
	void updateMenuBar();
	
	QList<CanvasController *> _canvasControllers;
	QPointer<CanvasController> _currentCanvas;
	
	ToolManager *_toolManager = 0;
	PaletteManager *_paletteManager = 0;
	
	QActionList _actions;
	WorkspaceModuleList _modules;
	
	QActionList _nullCanvasActions;
	CanvasModuleList _nullCanvasModules;
	
	ScopedQObjectPointer<WorkspaceView> _view;
	
	WorkspaceCanvasAreaController *_canvasAreaController = 0;
};

}

#endif // WORKSPACECONTROLLER_H
