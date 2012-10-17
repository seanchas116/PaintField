#ifndef WORKSPACECONTROLLER_H
#define WORKSPACECONTROLLER_H

#include <QObject>
#include <QPointer>

#include "canvascontroller.h"
#include "workspaceview.h"

namespace PaintField
{

class ToolManager;
class PaletteManager;
class ActionManager;
class WorkspaceMdiAreaController;

class WorkspaceController : public QObject
{
	Q_OBJECT
public:
	explicit WorkspaceController(QObject *parent = 0);
	
	ToolManager *toolManager() { return _toolManager; }
	PaletteManager *paletteManager() { return _paletteManager; }
	ActionManager *actionManager() { return _actionManager; }
	
	WorkspaceView *createView(QWidget *parent = 0);
	WorkspaceView *view() { return _view; }
	
	void addCanvas(CanvasController *canvas);
	
signals:
	
	void currentCanvasChanged(CanvasController *canvas);
	
	void canvasAboutToBeAdded(CanvasController *canvas);
	void canvasAdded(CanvasController *canvas);
	
	void canvasRemoved(CanvasController *canvas);
	void focused();
	
public slots:
	
	void setFocus();
	
	void setCurrentCanvas(CanvasController *canvas);
	
	void newCanvas();
	void openCanvas();
	
	bool tryClose();
	
protected:
	
	bool eventFilter(QObject *watched, QEvent *event);
	
private slots:
	
	void onCanvasSholudBeDeleted();
	void removeCanvas(CanvasController *canvas);
	
private:
	
	void createSidebars();
	void createSidebarInArea(const QVariantList &ids, Qt::DockWidgetArea area);
	void updateSidebars();
	void updateSidebarsForCanvas(CanvasController *canvas);
	void createMenuBar();
	void updateMenuBar();
	
	QList<CanvasController *> _canvasControllers;
	QPointer<CanvasController> _currentCanvas;
	
	ToolManager *_toolManager = 0;
	PaletteManager *_paletteManager = 0;
	ActionManager *_actionManager = 0;
	
	WorkspaceMdiAreaController *_mdiAreaController = 0;
	
	QPointer<WorkspaceView> _view;
};

}

#endif // WORKSPACECONTROLLER_H
