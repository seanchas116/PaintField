#include "application.h"

#include "workspacecontroller.h"

namespace PaintField
{

class MenuArranger
{
public:
	
	static QMenu *createMenu(const QVariantMap &order);
	static QMenuBar *createMenuBar(const QVariant &order);
	
	static void associateMenuWithActions(QMenu *menu, const QList<QAction *> &actions);
	static void associateMenuBarWithActions(QMenuBar *menuBar, const QList<QAction *> &actions);
};

QMenu *MenuArranger::createMenu(const QVariantMap &order)
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
					menu->addSeparator();
				else
				{
					QString text = app()->menuItemTitles()[id];
					if (text.isEmpty())
						text = id;
					
					WorkspaceMenuAction *action = new WorkspaceMenuAction(menu);
					action->setObjectName(id);
					action->setText(text);
					action->setShortcut(app()->actionShortcuts()[id]);
					menu->addAction(action);
				}
				break;
			}
			case QVariant::Map:
			{
				QMenu *childMenu = createMenu(child.toMap());
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

QMenuBar *MenuArranger::createMenuBar(const QVariant &order)
{
	QMenuBar *menuBar = new QMenuBar();
	
	QVariantList orders = order.toList();
	for (const QVariant &menuOrder : orders)
		menuBar->addMenu(createMenu(menuOrder.toMap()));
	
	return menuBar;
}

void MenuArranger::associateMenuWithActions(QMenu *menu, const QList<QAction *> &actions)
{
	for (QAction *action : menu->actions())
	{
		WorkspaceMenuAction *menuAction = qobject_cast<WorkspaceMenuAction *>(action);
		if (menuAction)
		{
			QAction *foundAction = findQObjectReverse(actions, menuAction->objectName());
			if (foundAction)
				menuAction->setBackendAction(foundAction);
			else
				menuAction->setEnabled(false);
		}
		else
		{
			if (action->menu())
				associateMenuWithActions(action->menu(), actions);
		}
	}
}

void MenuArranger::associateMenuBarWithActions(QMenuBar *menuBar, const QList<QAction *> &actions)
{
	for (QAction *action : menuBar->actions())
	{
		QMenu *menu = action->menu();
		if (menu)
			associateMenuWithActions(menu, actions);
	}
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
	
	connect(_mdiAreaController, SIGNAL(currentCanvasChanged(CanvasController*)), this, SLOT(setCurrentCanvas(CanvasController*)));
}

WorkspaceView *WorkspaceController::createView(QWidget *parent)
{
	WorkspaceView *view = new WorkspaceView(parent);
	_view = view;
	
	QMdiArea *mdiArea = _mdiAreaController->createView();
	view->setCentralWidget(mdiArea);
	
	createSidebars();
	updateSidebars();
	updateSidebarsForCanvas(_currentCanvas);
	
	createMenuBar();
	updateMenuBar();
	
	return view;
}

void WorkspaceController::newCanvas()
{
	CanvasController *controller = CanvasController::fromNew(this);
	
	if (controller)
	{
		addCanvas(controller);
		setCurrentCanvas(controller);
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

bool WorkspaceController::tryClose()
{
	for (CanvasController *controller : _canvasControllers)
	{
		if (!controller->closeCanvas())
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
			updateSidebarsForCanvas(canvas);
			updateMenuBar();
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
	connect(controller, SIGNAL(shouldBeClosed()), this, SLOT(onCanvasSholudBeClosed()));
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

void WorkspaceController::onCanvasSholudBeClosed()
{
	CanvasController *canvas = qobject_cast<CanvasController *>(sender());
	if (canvas && _canvasControllers.contains(canvas))
		removeCanvas(canvas);
}

void WorkspaceController::removeCanvas(CanvasController *canvas)
{
	if (_canvasControllers.contains(canvas))
	{
		if (_currentCanvas == canvas)
			setCurrentCanvas(0);
		
		_canvasControllers.removeOne(canvas);
		emit canvasRemoved(canvas);
		canvas->deleteLater();
	}
}

void WorkspaceController::createSidebars()
{
	QVariantMap orderMap = app()->panelOrder().toMap();
	
	createSidebarInArea(orderMap["left"].toList(), Qt::LeftDockWidgetArea);
	createSidebarInArea(orderMap["right"].toList(), Qt::RightDockWidgetArea);
}

void WorkspaceController::createSidebarInArea(const QVariantList &ids, Qt::DockWidgetArea area)
{
	for (SidebarFactory *factory : app()->sidebarFactories())
	{
		if (ids.contains(factory->objectName()))
		{
			_view->addSidebarFrame(factory->objectName(), factory->text(), area);
		}
	}
}

void WorkspaceController::updateSidebars()
{
	for (SidebarFactory *factory : app()->sidebarFactories())
		_view->setSidebar(factory->objectName(), factory->createSidebar(this, 0));
}

void WorkspaceController::updateSidebarsForCanvas(CanvasController *canvas)
{
	for (SidebarFactory *factory : app()->sidebarFactories())
		_view->setSidebar(factory->objectName(), factory->createSidebarForCanvas(canvas, 0));
}

void WorkspaceController::createMenuBar()
{
	_view->setMenuBar(MenuArranger::createMenuBar(app()->menuBarOrder()));
}

void WorkspaceController::updateMenuBar()
{
	QList<QAction *> actions = app()->actionManager()->actions() + _actionManager->actions();
	if (_currentCanvas)
		actions += _currentCanvas->actionManager()->actions();
	
	MenuArranger::associateMenuBarWithActions(_view->menuBar(), actions);
}

}
