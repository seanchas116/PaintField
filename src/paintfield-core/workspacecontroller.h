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
class CanvasSplitAreaController;

class WorkspaceController : public QObject
{
	Q_OBJECT
public:
	explicit WorkspaceController(QObject *parent = 0);
	~WorkspaceController();
	
	ToolManager *toolManager();
	PaletteManager *paletteManager();
	
	WorkspaceView *view();
	void updateView();
	
	void addModules(const WorkspaceModuleList &modules);
	WorkspaceModuleList modules();
	
	void addActions(const QActionList &actions);
	QActionList actions();
	
	void addNullCanvasModules(const CanvasModuleList &modules);
	CanvasModuleList nullCanvasModules();
	
	void addNullCanvasActions(const QActionList &actions);
	QActionList nullCanvasActions();
	
	void addAndShowCanvas(CanvasController *canvas);
	
	/**
	 * Adds a canvas.
	 * @param canvas
	 */
	void addCanvas(CanvasController *canvas);
	
	/**
	 * Removes a canvas.
	 * The canvas is not deleted.
	 * @param canvas
	 */
	void removeCanvas(CanvasController *canvas);
	
	QList<CanvasController *> canvases();
	
signals:
	
	void currentCanvasChanged(CanvasController *canvas);
	
	void canvasShowRequested(CanvasController *canvas);
	
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
	
	void newCanvasFromImageFile();
	
	/**
	 * Opens a document and adds a canvas of it.
	 */
	void openCanvas();
	
	void openCanvasFromFilepath(const QString &filepath);
	
	/**
	 * Try to close all canvases.
	 * @return If succeeded
	 */
	bool tryClose();
	
protected:
	
	bool eventFilter(QObject *watched, QEvent *event);
	
private slots:
	
	void deleteCanvas(CanvasController *canvas);
	
private:
	
	QActionList currentCanvasActions();
	CanvasModuleList currentCanvasModules();
	
	void updateWorkspaceItems();
	void updateWorkspaceItemsForCanvas(CanvasController *canvas);
	void updateMenuBar();
	
	class Data;
	Data *d;
};

}

#endif // WORKSPACECONTROLLER_H
