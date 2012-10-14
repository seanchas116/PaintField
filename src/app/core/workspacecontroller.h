#ifndef WORKSPACECONTROLLER_H
#define WORKSPACECONTROLLER_H

#include <QObject>
#include <QMenuBar>

#include "canvascontroller.h"
#include "workspacecontroller.h"
#include "sidebarfactory.h"
#include "toolmanager.h"
#include "palettemanager.h"

#include "workspaceview.h"

#include "workspacemdiareacontroller.h"

namespace PaintField
{

QMenuBar *createAndArrangeMenuBar(const QList<QAction *> &actions, const QVariant &order);
QMenu *createAndArrangeMenu(const QList<QAction *> &actions, const QVariantMap &order);

class WorkspaceController : public QObject
{
	Q_OBJECT
public:
	explicit WorkspaceController(QObject *parent = 0);
	
	ToolManager *toolManager() { return _toolManager; }
	PaletteManager *paletteManager() { return _paletteManager; }
	ActionManager *actionManager() { return _actionManager; }
	
	WorkspaceView *createView(QWidget *parent = 0);
	
signals:
	
	void currentCanvasChanged(CanvasController *controller);
	void canvasAdded(CanvasController *controller);
	void canvasRemoved(CanvasController *controller);
	void focused();
	
public slots:
	
	void setFocus() { _view->setFocus(); }
	
	bool tryCanvasClose(CanvasController *controller);
	void setCurrentCanvas(CanvasController *controller);
	
	void newCanvas();
	void openCanvas();
	
	bool tryClose();
	
protected:
	
	bool eventFilter(QObject *watched, QEvent *event);
	
	void addCanvas(CanvasController *controller);
	
	CanvasController *controllerForCanvasView(CanvasView *canvas);
	
private:
	
	void prepareSidebars();
	void prepareSidebarInArea(const QVariantList &ids, Qt::DockWidgetArea area);
	void createSidebars();
	void createSidebarsForCanvas(CanvasController *canvas);
	void createMenuBar();
	
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
