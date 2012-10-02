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
	
	~WorkspaceController();
	
	void addAction(QAction *action)
	{
		_view->addAction(action);
		_actions << action;
	}
	void addAction(QAction *action, const QString &id)
	{
		action->setObjectName(id);
		addAction(action);
	}
	void addAction(QAction *action, const QString &id, const QObject *receiver, const char *triggerSlot)
	{
		connect(action, SIGNAL(triggered()), receiver, triggerSlot);
		addAction(action, id);
	}
	
	void addPanel(QWidget *panel) { _panels << panel; }
	void addPanel(QWidget *panel, const QString &id)
	{
		panel->setObjectName(id);
		addPanel(panel);
	}
	
	ToolManager *toolManager() { return _toolManager; }
	PaletteManager *paletteManager() { return _paletteManager; }
	
signals:
	
	void currentCanvasChanged(Canvas *canvas);
	void canvasAdded(Canvas *canvas);
	void canvasRemoved(Canvas *canvas);
	
	void focused();
	
public slots:
	
	void show() { _view->show(); }
	void setFocus() { _view->setFocus(); }
	
	void requestCanvasClose(Canvas *canvas);
	void changeCurrentCanvas(Canvas *canvas);
	
	void newCanvas();
	void openCanvas();
	
protected:
	
	bool eventFilter(QObject *watched, QEvent *event);
	
	void addCanvasController(CanvasController *controller);
	
	CanvasController *controllerForCanvas(Canvas *canvas);
	
private:
	
	QMenuBar *arrangeMenuBar(const QVariantList &orders);
	QMenu *arrangeMenu(const QVariantMap &order);
	QAction *actionForId(const QString &id);
	
	QList<CanvasController *> _canvasControllers;
	CanvasController *_currentCanvasController;
	
	ToolManager *_toolManager;
	PaletteManager *_paletteManager;
	
	QList<QAction *> _actions;
	QList<QWidget *> _panels;
	
	QList<QAction *> _dummyActions;
	
	QPointer<WorkspaceView> _view;
};

}

#endif // WORKSPACECONTROLLER_H
