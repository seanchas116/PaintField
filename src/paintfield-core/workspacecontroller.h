#ifndef WORKSPACECONTROLLER_H
#define WORKSPACECONTROLLER_H

#include <QObject>
#include <QPointer>

#include "module.h"
#include "global.h"

namespace PaintField
{

class Canvas;
class WorkspaceView;
class WorkspaceModule;
class ToolManager;
class PaletteManager;
class CanvasSplitAreaController;

class Workspace : public QObject
{
	Q_OBJECT
public:
	explicit Workspace(QObject *parent = 0);
	~Workspace();
	
	ToolManager *toolManager();
	PaletteManager *paletteManager();
	
	WorkspaceView *view();
	
	void addModules(const WorkspaceModuleList &modules);
	WorkspaceModuleList modules();
	
	void addActions(const QActionList &actions);
	QActionList actions();
	
	void addNullCanvasModules(const CanvasModuleList &modules);
	CanvasModuleList nullCanvasModules();
	
	void addNullCanvasActions(const QActionList &actions);
	QActionList nullCanvasActions();
	
	void addAndShowCanvas(Canvas *canvas);
	
	/**
	 * Adds a canvas.
	 * @param canvas
	 */
	void addCanvas(Canvas *canvas);
	
	/**
	 * Removes a canvas.
	 * The canvas is not deleted.
	 * @param canvas
	 */
	void removeCanvas(Canvas *canvas);
	
	QList<Canvas *> canvases();
	
signals:
	
	void currentCanvasChanged(Canvas *canvas);
	
	void canvasShowRequested(Canvas *canvas);
	
	void canvasAboutToBeRemoved(Canvas *canvas);
	void focused();
	
	void shouldBeDeleted(Workspace *workspace);
	
public slots:
	
	void setFocus();
	
	/**
	 * Sets the current canvas.
	 * @param canvas
	 */
	void setCurrentCanvas(Canvas *canvas);
	
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
	
private slots:
	
	void deleteCanvas(Canvas *canvas);
	
private:
	
	QActionList currentCanvasActions();
	CanvasModuleList currentCanvasModules();
	
	void updateWorkspaceItems();
	void updateWorkspaceItemsForCanvas(Canvas *canvas);
	void updateMenuBar();
	
	class Data;
	Data *d;
};

}

#endif // WORKSPACECONTROLLER_H
