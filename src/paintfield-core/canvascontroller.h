#ifndef CANVASCONTROLLER_H
#define CANVASCONTROLLER_H

#include <QObject>
#include <QPointer>

#include "document.h"
#include "module.h"

namespace PaintField
{

class CanvasModule;
class Workspace;

class Canvas : public QObject
{
	Q_OBJECT
public:
	
	/**
	 * Constructs a canvas controller.
	 * @param document
	 * @param parent
	 */
	Canvas(Document *document, Workspace *parent = 0);
	
	Canvas(Canvas *other, Workspace *parent = 0);
	
	~Canvas();
	
	/**
	 * Shows a dialog, creates a new file and creates a controller from it.
	 * @param parent The new controller's parent
	 * @return The created controller
	 */
	static Canvas *fromNew();
	
	/**
	 * Shows a dialog, opens a file and creates a controller from it.
	 * @param parent The new controller's parent
	 * @return The created controller
	 */
	static Canvas *fromOpen();
	
	static Canvas *fromNewFromImageFile();
	
	static Canvas *fromFile(const QString &path);
	static Canvas *fromSavedFile(const QString &path);
	static Canvas *fromImageFile(const QString &path);
	
	void setWorkspace(Workspace *workspace);
	
	/**
	 * @return The workspace controller which have the canvas controller
	 */
	Workspace *workspace();
	
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
	
	/**
	 * This function must be used only by CanvasView.
	 * @param view
	 */
	void setView(CanvasView *view);
	
	CanvasView *view();
	
	virtual void onSetCurrent();
	
	double scale() const;
	double rotation() const;
	QPoint translation() const;
	
	Tool *tool();
	
signals:
	
	/**
	 * Emitted when the canvas should be deleted.
	 */
	void shouldBeDeleted(Canvas *canvas);
	
	void scaleChanged(double scale);
	void rotationChanged(double rotation);
	void translationChanged(const QPoint &translation);
	
	void toolChanged(Tool *tool);
	
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
	
	void setScale(double scale);
	void setRotation(double rotation);
	void setTranslation(const QPoint &translation);
	
private slots:
	
	void onToolChanged(const QString &name);
	
private:
	
	void commonInit();
	
	class Data;
	Data *d;
};

}

#endif // CANVASCONTROLLER_H
