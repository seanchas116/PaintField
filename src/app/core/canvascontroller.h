#ifndef CANVASCONTROLLER_H
#define CANVASCONTROLLER_H

#include <QObject>
#include <QAction>
#include <QWidget>
#include <QMenuBar>

#include "util.h"
#include "canvasview.h"
#include "actionmanager.h"

namespace PaintField
{

class WorkspaceController;

class CanvasController : public QObject
{
	Q_OBJECT
public:
	
	/**
	 * Constructs a canvas controller.
	 * @param document
	 * @param parent
	 */
	CanvasController(Document *document, WorkspaceController *parent = 0);
	
	/**
	 * Shows a dialog, creates a new file and creates a controller from it.
	 * @param parent The new controller's parent
	 * @return The created controller
	 */
	static CanvasController *fromNew(WorkspaceController *parent = 0);
	
	/**
	 * Shows a dialog, opens a file and creates a controller from it.
	 * @param parent The new controller's parent
	 * @return The created controller
	 */
	static CanvasController *fromOpen(WorkspaceController *parent = 0);
	
	/**
	 * @return The workspace controller which have the canvas controller
	 */
	WorkspaceController *workspace() { return reinterpret_cast<WorkspaceController *>(parent()); }
	
	/**
	 * @return The canvas view
	 */
	CanvasView *view() { return _view.data(); }
	
	/**
	 * @return The document the canvas handles
	 */
	Document *document() { return _view->document(); }
	
	/**
	 * @return The actions which belongs to the controller.
	 */
	QList<QAction *> actions() { return _actions; }
	
	ActionManager *actionManager() { return _actionManager; }
	
	CanvasView *createView(QWidget *parent = 0);
	
signals:
	
public slots:
	
	bool saveAsCanvas();
	bool saveCanvas();
	bool closeCanvas();
	
private:
	
	Document *_document;
	QPointer<CanvasView> _view;
	ActionManager *_actionManager;
	QList<QAction *> _actions;
};

}

#endif // CANVASCONTROLLER_H
