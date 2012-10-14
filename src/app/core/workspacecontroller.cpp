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
	// create tool / palette manager
	
	_toolManager = new ToolManager(this);
	_paletteManager = new PaletteManager(this);
	_actionManager = new ActionManager(this);
	
	// create actions
	
	_actionManager->addAction("paintfield.file.new", this, SLOT(newCanvas()));
	_actionManager->addAction("paintfield.file.open", this, SLOT(openCanvas()));
	
	_mdiAreaController = new WorkspaceMdiAreaController(this);
	
	connect(this, SIGNAL(canvasAdded(CanvasController*)), _mdiAreaController, SLOT(addCanvas(CanvasController*)));
	connect(this, SIGNAL(canvasRemoved(CanvasController*)), _mdiAreaController, SLOT(removeCanvas(CanvasController*)));
	connect(this, SIGNAL(currentCanvasChanged(CanvasController*)), _mdiAreaController, SLOT(setCurrentCanvas(CanvasController*)));
	
	connect(_mdiAreaController, SIGNAL(canvasCloseRequested(CanvasController*)), this, SLOT(tryCanvasClose(CanvasController*)));
	connect(_mdiAreaController, SIGNAL(currentCanvasChanged(CanvasController*)), this, SLOT(setCurrentCanvas(CanvasController*)));
}

WorkspaceView *WorkspaceController::createView(QWidget *parent)
{
	WorkspaceView *view = new WorkspaceView(parent);
	_view = view;
	
	QMdiArea *mdiArea = _mdiAreaController->createView();
	view->setCentralWidget(mdiArea);
	
	prepareSidebars();
	createSidebars();
	createSidebarsForCanvas(_currentCanvas);
	createMenuBar();
	
	return view;
}

void WorkspaceController::newCanvas()
{
	CanvasController *controller = CanvasController::fromNew(this);
	
	if (controller)
	{
		addCanvas(controller);
		//setCurrentCanvas(controller);
	}
}

void WorkspaceController::openCanvas()
{
	CanvasController *controller = CanvasController::fromOpen(this);
	
	if (controller)
	{
		addCanvas(controller);
		setCurrentCanvas(controller);
	}
}

bool WorkspaceController::tryCanvasClose(CanvasController *controller)
{
	if (controller && controller->closeCanvas())
	{
		emit canvasRemoved(controller);
		controller->deleteLater();
		_canvasControllers.removeOne(controller);
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

void WorkspaceController::setCurrentCanvas(CanvasController *canvas)
{
	if (_currentCanvas != canvas)
	{
		_currentCanvas = canvas;
		emit currentCanvasChanged(canvas);
		
		if (_view)
		{
			createSidebarsForCanvas(canvas);
			createMenuBar();
		}
	}
}

bool WorkspaceController::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == _view)
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

void WorkspaceController::prepareSidebars()
{
	QVariantMap orderMap = app()->panelOrder().toMap();
	
	prepareSidebarInArea(orderMap["left"].toList(), Qt::LeftDockWidgetArea);
	prepareSidebarInArea(orderMap["right"].toList(), Qt::RightDockWidgetArea);
}

void WorkspaceController::prepareSidebarInArea(const QVariantList &ids, Qt::DockWidgetArea area)
{
	for (SidebarFactory *factory : app()->sidebarFactories())
	{
		if (ids.contains(factory->objectName()))
		{
			_view->addSidebarFrame(factory->objectName(), factory->text(), area);
		}
	}
}

void WorkspaceController::createSidebars()
{
	for (SidebarFactory *factory : app()->sidebarFactories())
		_view->setSidebar(factory->objectName(), factory->createSidebar(this, 0));
}

void WorkspaceController::createSidebarsForCanvas(CanvasController *canvas)
{
	for (SidebarFactory *factory : app()->sidebarFactories())
		_view->setSidebar(factory->objectName(), factory->createSidebarForCanvas(canvas, 0));
}

void WorkspaceController::createMenuBar()
{
	QList<QAction *> actions = app()->actionManager()->actions() + _actionManager->actions();
	if (_currentCanvas)
		actions += _currentCanvas->actionManager()->actions();
	
	if (_view->menuBar())
		_view->menuBar()->deleteLater();
	
	_view->setMenuBar(createAndArrangeMenuBar(actions, app()->menuBarOrder()));
}

}
