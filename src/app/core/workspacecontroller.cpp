#include "application.h"

#include "workspacecontroller.h"

namespace PaintField
{

QMenuBar *createAndArrangeMenuBar(const QList<QAction *> &actions, const QVariant &order)
{
	QMenuBar *menuBar = new QMenuBar();
	
	QVariantList orders = order.toList();
	for (const QVariant &menuOrder : orders)
		menuBar->addMenu(createAndArrangeMenu(actions, menuOrder.toMap()));
	
	return menuBar;
}

QMenu *createAndArrangeMenu(const QList<QAction *> &actions, const QVariantMap &order)
{
	QString menuId = order["menu"].toString();
	if (menuId.isEmpty())
		return 0;
	
	QString menuTitle = app()->menuItemTitles()[menuId];
	
	if (menuTitle.isEmpty())
		menuTitle = order["menu"].toString();
	
	QMenu *menu = new QMenu(menuTitle);
	
	QVariantList children = order["children"].toList();
	
	for (const QVariant &child : children)
	{
		switch (child.type())
		{
			case QVariant::String:
			{
				QString id = child.toString();
				if (id.isEmpty())
				{
					menu->addSeparator();
				}
				else
				{
					QString text = app()->menuItemTitles()[id];
					if (text.isEmpty())
						text = id;
					
					QAction *action = findQObjectReverse(actions, id);
					
					if (action == nullptr)
					{
						action = new QAction(menu);
						action->setEnabled(false);
					}
					
					action->setText(text);
					action->setShortcut(app()->actionShortcuts()[id]);
					menu->addAction(action);
				}
				break;
			}
			case QVariant::Map:
			{
				QVariantMap subOrder = child.toMap();
				QMenu *childMenu = createAndArrangeMenu(actions, subOrder);
				if (childMenu)
					menu->addMenu(childMenu);
				break;
			}
			default:
				break;
		}
	}
	
	return menu;
}

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
	
	_actionManager->addAction("paintfield.file.new", this, SLOT(newCanvas()));
	_actionManager->addAction("paintfield.file.open", this, SLOT(openCanvas()));
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

bool WorkspaceController::tryCanvasClose(CanvasController *controller)
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

bool WorkspaceController::tryClose()
{
	for (CanvasController *controller : _canvasControllers )
	{
		if (tryCanvasClose(controller) == false)
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
	QList<QAction *> actions = app()->actionManager()->actions() + _actionManager->actions();
	
	if (_currentCanvasController)
		actions += _currentCanvasController->actionManager()->actions();
	
	_view->setMenuBar(createAndArrangeMenuBar(actions, app()->menuBarOrder()));
}

}
