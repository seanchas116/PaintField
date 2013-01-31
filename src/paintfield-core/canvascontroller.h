#ifndef CANVASCONTROLLER_H
#define CANVASCONTROLLER_H

#include <QObject>
#include <QPointer>

#include "document.h"
#include "module.h"

namespace PaintField
{

class CanvasModule;
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
	static CanvasController *fromNew();
	
	/**
	 * Shows a dialog, opens a file and creates a controller from it.
	 * @param parent The new controller's parent
	 * @return The created controller
	 */
	static CanvasController *fromOpen();
	
	static CanvasController *fromNewFromImageFile();
	
	static CanvasController *fromFile(const QString &path);
	static CanvasController *fromSavedFile(const QString &path);
	static CanvasController *fromImageFile(const QString &path);
	
	void setWorkspace(WorkspaceController *workspace);
	
	/**
	 * @return The workspace controller which have the canvas controller
	 */
	WorkspaceController *workspace();
	
	/**
	 * @return The document the canvas handles
	 */
	Document *document();
	
	LayerModel *layerModel() { return document()->layerModel(); }
	
	QItemSelectionModel *selectionModel();
	
	void addActions(const QActionList &actions);
	
	/**
	 * @return The actions which belongs to the controller.
	 */
	QActionList actions();
	
	void addModules(const CanvasModuleList &modules);
	CanvasModuleList modules();
	
	CanvasView *view();
	
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
	
	void newCanvasIntoDocument();
	
	bool exportCanvas();
	
private slots:
	
	void onToolChanged(const QString &name);
	
private:
	
	void commonInit();
	
	class Data;
	Data *d;
};

}

#endif // CANVASCONTROLLER_H
