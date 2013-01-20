#ifndef CANVASCONTROLLER_H
#define CANVASCONTROLLER_H

#include <QObject>
#include <QPointer>

#include "smartpointer.h"
#include "module.h"
#include "canvasview.h"

namespace PaintField
{

class CanvasModule;
class Document;
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
	
	CanvasController(CanvasController *other, WorkspaceController *parent = 0);
	
	~CanvasController();
	
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
	
	static CanvasController *fromNewFromImageFile(WorkspaceController *parent);
	
	static CanvasController *fromFile(const QString &path, WorkspaceController *parent);
	static CanvasController *fromSavedFile(const QString &path, WorkspaceController *parent);
	static CanvasController *fromImageFile(const QString &path, WorkspaceController *parent);
	
	void setWorkspace(WorkspaceController *workspace);
	
	/**
	 * @return The workspace controller which have the canvas controller
	 */
	WorkspaceController *workspace() { return _workspace; }
	
	/**
	 * @return The document the canvas handles
	 */
	Document *document() { return _document.obj(); }
	
	LayerModel *layerModel() { return _document->layerModel(); }
	
	QItemSelectionModel *selectionModel() { return _selectionModel; }
	
	void addActions(const QActionList &actions) { _actions += actions; }
	
	/**
	 * @return The actions which belongs to the controller.
	 */
	QActionList actions() { return _actions; }
	
	void addModules(const CanvasModuleList &modules);
	CanvasModuleList modules() { return _modules; }
	
	CanvasView *view() { return _view.data(); }
	
	virtual void onSetCurrent();
	
signals:
	
	/**
	 * Emitted when the canvas should be deleted.
	 */
	void shouldBeDeleted(CanvasController *canvas);
	
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
	
	bool exportCanvas();
	
private slots:
	
	void onToolChanged(const QString &name);
	
private:
	
	void commonInit();
	
	WorkspaceController *_workspace = 0;
	
	CountableSharedQObjectPointer<Document> _document;
	QItemSelectionModel *_selectionModel = 0;
	
	ScopedQObjectPointer<CanvasView> _view;
	QActionList _actions;
	CanvasModuleList _modules;
};

}

#endif // CANVASCONTROLLER_H
