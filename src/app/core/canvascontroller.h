#ifndef CANVASCONTROLLER_H
#define CANVASCONTROLLER_H

#include <QObject>
#include <QPointer>

class QAction;

namespace PaintField
{

class Document;
class CanvasView;
class ActionManager;
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
	CanvasController(Document *document, WorkspaceController *parent);
	
	/**
	 * Shows a dialog, creates a new file and creates a controller from it.
	 * @param parent The new controller's parent
	 * @return The created controller
	 */
	static CanvasController *fromNew(WorkspaceController *parent);
	
	/**
	 * Shows a dialog, opens a file and creates a controller from it.
	 * @param parent The new controller's parent
	 * @return The created controller
	 */
	static CanvasController *fromOpen(WorkspaceController *parent);
	
	/**
	 * @return The workspace controller which have the canvas controller
	 */
	WorkspaceController *workspace() { return reinterpret_cast<WorkspaceController *>(parent()); }
	
	/**
	 * @return The document the canvas handles
	 */
	Document *document() { return _document; }
	
	/**
	 * @return The actions which belongs to the controller.
	 */
	QList<QAction *> actions() { return _actions; }
	
	ActionManager *actionManager() { return _actionManager; }
	
	CanvasView *createView(QWidget *parent = 0);
	
signals:
	
	/**
	 * Emitted when the canvas should be deleted.
	 */
	void shouldBeDeleted();
	
public slots:
	
	/**
	 * Shows a dialog and saves the canvas into a new file.
	 * @return false if cancelled or failed
	 */
	bool saveAsCanvas();
	
	/**
	 * Shows a dialog and saves the canvas.
	 * @return false if cancelled or failed
	 */
	bool saveCanvas();
	
	/**
	 * Shows a warning dialog and emits shouldBeClosed() if not cancelled.
	 * @return false if cancelled
	 */
	bool closeCanvas();
	
private:
	
	Document *_document = 0;
	QPointer<CanvasView> _view;
	ActionManager *_actionManager = 0;
	QList<QAction *> _actions;
};

}

#endif // CANVASCONTROLLER_H
