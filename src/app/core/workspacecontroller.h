#ifndef WORKSPACECONTROLLER_H
#define WORKSPACECONTROLLER_H

#include <QObject>
#include <QMenuBar>

#include "canvascontroller.h"
#include "workspaceview.h"

#include "toolmanager.h"
#include "palettemanager.h"

namespace PaintField
{

class WorkspaceController : public QObject
{
	Q_OBJECT
public:
	explicit WorkspaceController(QObject *parent = 0);
	
	void addPanel(QWidget *panel) { _panels << panel; }
	void addPanel(QWidget *panel, const QString &id)
	{
		panel->setObjectName(id);
		addPanel(panel);
	}
	
	ToolManager *toolManager() { return _toolManager; }
	PaletteManager *paletteManager() { return _paletteManager; }
	
signals:
	
	void currentCanvasChanged(CanvasController *controller);
	void currentCanvasViewChanged(CanvasView *view);
	
	void canvasAdded(CanvasController *controller);
	void canvasViewAdded(CanvasView *view);
	
	void canvasRemoved(CanvasController *controller);
	void canvasViewRemoved(CanvasView *view);
	
	void focused();
	
public slots:
	
	void show();
	void setFocus() { _view->setFocus(); }
	
	bool requestCanvasViewClose(CanvasView *canvas)
	{
		CanvasController *controller = controllerForCanvasView(canvas);
		if (controller)
			return requestCanvasClose(controller);
		else
			return false;
	}
	
	bool requestCanvasClose(CanvasController *controller);
	
	void changeCurrentCanvasView(CanvasView *canvas)
	{
		CanvasController *controller = controllerForCanvasView(canvas);
		if (controller)
			changeCurrentCanvas(controller);
	}
	
	void changeCurrentCanvas(CanvasController *controller);
	
	void newCanvas();
	void openCanvas();
	
	bool requestClose();
	
protected:
	
	bool eventFilter(QObject *watched, QEvent *event);
	
	void addCanvas(CanvasController *controller);
	
	CanvasController *controllerForCanvasView(CanvasView *canvas);
	
private:
	
	void arrangePanels();
	void arrangeMenuBar();
	
	void arrangePanelsInArea(const QWidgetList &panels, Qt::DockWidgetArea area, const QVariantList &list);
	QMenu *createAndArrangeMenu(ActionManager *actionManager, const QVariantMap &order);
	QAction *actionForId(const QString &id);
	
	QList<CanvasController *> _canvasControllers;
	QPointer<CanvasController> _currentCanvasController;
	
	ToolManager *_toolManager;
	PaletteManager *_paletteManager;
	ActionManager *_actionManager;
	QList<QWidget *> _panels;
	
	QList<QAction *> _dummyActions;
	
	ScopedQObjectPointer<WorkspaceView> _view;
};

}

#endif // WORKSPACECONTROLLER_H
