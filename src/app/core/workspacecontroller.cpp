#include "application.h"

#include "workspacecontroller.h"

namespace PaintField
{

WorkspaceController::WorkspaceController(QObject *parent) :
    QObject(parent)
{
	// create view
	
	_view .reset(new WorkspaceView());
	
	CanvasMdiArea *mdiArea = new CanvasMdiArea();
	
	connect(this, SIGNAL(canvasViewAdded(CanvasView*)), mdiArea, SLOT(addCanvas(CanvasView*)));
	connect(this, SIGNAL(canvasViewRemoved(CanvasView*)), mdiArea, SLOT(removeCanvas(CanvasView*)));
	connect(this, SIGNAL(currentCanvasViewChanged(CanvasView*)), mdiArea, SLOT(setCurrentCanvas(CanvasView*)));
	
	connect(mdiArea, SIGNAL(currentCanvasChanged(CanvasView*)), this, SLOT(changeCurrentCanvasView(CanvasView*)));
	
	_view->setCentralWidget(mdiArea);
	
	// create tool / palette manager
	
	_toolManager = new ToolManager(this);
	_paletteManager = new PaletteManager(this);
	_actionManager = new ActionManager(this);
	
	// create actions
	
	_actionManager->addAction("paintfield.file.new", this, SLOT(newCanvas()), tr("New..."));
	_actionManager->addAction("paintfield.file.open", this, SLOT(openCanvas()), tr("Open..."));
}

void WorkspaceController::show()
{
	arrangePanels();
	arrangeMenuBar();
	_view->show();
}

void WorkspaceController::newCanvas()
{
	CanvasController *controller = CanvasController::fromNew(this);
	
	if (controller)
	{
		addCanvas(controller);
		changeCurrentCanvas(controller);
	}
}

void WorkspaceController::openCanvas()
{
	CanvasController *controller = CanvasController::fromOpen(this);
	
	if (controller)
	{
		addCanvas(controller);
		changeCurrentCanvas(controller);
	}
}

bool WorkspaceController::requestCanvasClose(CanvasController *controller)
{
	if (controller && controller->closeCanvas())
	{
		emit canvasViewRemoved(controller->view());
		emit canvasRemoved(controller);
		controller->deleteLater();
		return true;
	}
	return false;
}

bool WorkspaceController::requestClose()
{
	for (CanvasController *controller : _canvasControllers )
	{
		if (requestCanvasClose(controller) == false)
			return false;
	}
	deleteLater();
	return true;
}

void WorkspaceController::changeCurrentCanvas(CanvasController *controller)
{
	_currentCanvasController = controller;
	emit currentCanvasChanged(controller);
	
	arrangeMenuBar();
}

bool WorkspaceController::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == _view.data())
	{
		if (event->type() == QEvent::FocusIn)
		{
			emit focused();
			return true;
		}
	}
	
	return false;
}

void WorkspaceController::addCanvas(CanvasController *controller)
{
	_canvasControllers << controller;
	emit canvasViewAdded(controller->view());
	emit canvasAdded(controller);
}

CanvasController *WorkspaceController::controllerForCanvasView(CanvasView *canvas)
{
	foreach (auto controller, _canvasControllers)
	{
		if (controller->view() == canvas)
			return controller;
	}
	
	return 0;
}

void WorkspaceController::arrangePanels()
{
	QVariantMap orderMap = app()->panelOrder().toMap();
	
	arrangePanelsInArea(_panels, Qt::LeftDockWidgetArea, orderMap["left"].toList());
	arrangePanelsInArea(_panels, Qt::RightDockWidgetArea, orderMap["right"].toList());
}

void WorkspaceController::arrangePanelsInArea(const QWidgetList &panels, Qt::DockWidgetArea area, const QVariantList &list)
{
	for (const QVariant &item : list)
	{
		QString id = item.toString();
		if (id.isEmpty())
			continue;
		for (QWidget *panel : panels)
		{
			if (panel->objectName() == id)
				_view->addPanel(area, panel);
		}
	}
}

void WorkspaceController::arrangeMenuBar()
{
	QMenuBar *menuBar = new QMenuBar();
	
	QVariantList orders = app()->menuBarOrder().toList();
	foreach (const QVariant &order, orders)
		menuBar->addMenu(createAndArrangeMenu(_actionManager, order.toMap()));
	
	_view->setMenuBar(menuBar);
}

QMenu *WorkspaceController::createAndArrangeMenu(ActionManager *actionManager, const QVariantMap &order)
{
	QMenu *menu = new QMenu(app()->menuItemTitles()[order["title"].toString()]);
	
	QVariantList children = order["children"].toList();
	
	foreach (const QVariant &child, children)
	{
		QVariantMap subOrder = child.toMap();
		
		if (subOrder.size()) // child is menu
		{
			menu->addMenu(createAndArrangeMenu(actionManager, subOrder));
			continue;
		}
		
		QString id = child.toString();
		
		if (!id.isEmpty())	// child is action
		{
			QString text = app()->menuItemTitles()[id];
			if (text.isEmpty())
				text = id;
			
			QAction *action = actionForId(id);
			
			if (!action)
			{
				action = new QAction(this);
				_dummyActions << action;
			}
			
			action->setText(text);
			action->setShortcut(app()->actionShortcuts()[id]);
			menu->addAction(action);
			
			continue;
		}
		
		menu->addSeparator();	// child is not an string nor an object
	}
	
	return menu;
}

QAction *WorkspaceController::actionForId(const QString &id)
{
	QList<ActionManager *> actionManagers;
	
	actionManagers << app()->actionManager();
	actionManagers << _actionManager;
	
	if (_currentCanvasController)
		actionManagers << _currentCanvasController->actionManager();
	
	for (ActionManager *manager : actionManagers)
	{
		QAction *action = manager->actionForId(id);
		if (action)
			return action;
	}
	
	return 0;
}

}
