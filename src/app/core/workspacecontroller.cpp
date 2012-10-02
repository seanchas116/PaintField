#include "application.h"

#include "workspacecontroller.h"

namespace PaintField
{

WorkspaceController::WorkspaceController(QObject *parent) :
    QObject(parent)
{
	// create view
	
	_view = new WorkspaceView();
	
	CanvasMdiArea *mdiArea = new CanvasMdiArea();
	
	connect(this, SIGNAL(canvasAdded(Canvas*)), mdiArea, SLOT(addCanvas(Canvas*)));
	connect(this, SIGNAL(canvasRemoved(Canvas*)), mdiArea, SLOT(removeCanvas(Canvas*)));
	connect(this, SIGNAL(currentCanvasChanged(Canvas*)), mdiArea, SLOT(setCurrentCanvas(Canvas*)));
	
	connect(mdiArea, SIGNAL(currentCanvasChanged(Canvas*)), this, SLOT(changeCurrentCanvas(Canvas*)));
	
	_view->setCentralWidget(mdiArea);
	
	// create tool / palette manager
	
	_toolManager = new ToolManager(this);
	_paletteManager = new PaletteManager(this);
	
	// create actions
	
	addAction(new QAction(this), "newDocument", this, SLOT(newCanvas()));
	addAction(new QAction(this), "openDocument", this, SLOT(openCanvas()));
}

WorkspaceController::~WorkspaceController()
{
	_view->deleteLater();
}

void WorkspaceController::newCanvas()
{
	CanvasController *controller = CanvasController::fromNew(this);
	
	if (controller)
	{
		addCanvasController(controller);
		changeCurrentCanvas(controller->canvas());
	}
}

void WorkspaceController::openCanvas()
{
	CanvasController *controller = CanvasController::fromOpen(this);
	
	if (controller)
	{
		addCanvasController(controller);
		changeCurrentCanvas(controller->canvas());
	}
}

void WorkspaceController::changeCurrentCanvas(Canvas *canvas)
{
	if (_currentCanvasController->canvas() == canvas)
		return;
	
	CanvasController *controller = controllerForCanvas(canvas);
	if (!controller)
		return;
	
	_currentCanvasController = controller;
	emit currentCanvasChanged(controller->canvas());
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

void WorkspaceController::addCanvasController(CanvasController *controller)
{
	_canvasControllers << controller;
	emit canvasAdded(controller->canvas());
}

CanvasController *WorkspaceController::controllerForCanvas(Canvas *canvas)
{
	foreach (auto controller, _canvasControllers)
	{
		if (controller->canvas() == canvas)
			return controller;
	}
	
	return 0;
}

QMenuBar *WorkspaceController::arrangeMenuBar(const QVariantList &orders)
{
	QMenuBar *menuBar = new QMenuBar();
	
	foreach (const QVariant &order, orders)
		menuBar->addMenu(arrangeMenu(order.toMap()));
	
	return menuBar;
}

QMenu *WorkspaceController::arrangeMenu(const QVariantMap &order)
{
	QMenu *menu = new QMenu(app()->menuTitles()[order["title"].toString()]);
	
	QVariantList children = order["children"].toList();
	
	foreach (const QVariant &child, children)
	{
		QVariantMap subOrder = child.toMap();
		
		if (subOrder.size()) // child is menu
		{
			menu->addMenu(arrangeMenu(subOrder));
			continue;
		}
		
		QString id = child.toString();
		
		if (!id.isEmpty())	// child is action
		{
			QAction *action = actionForId(id);
			
			if (!action)
			{
				action = new QAction(this);
				_dummyActions << action;
			}
			
			action->setText(app()->actionTitles()[id]);
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
	QListIterator<QAction *> iter(_actions);
	iter.toBack();
	
	while (iter.hasPrevious())
	{
		QAction *action = iter.previous();
		
		if (action->objectName() == id)
			return action;
	}
	
	return 0;
}

}
