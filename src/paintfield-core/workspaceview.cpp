#include <QtGui>
#include "widgets/docktabmotherwidget.h"
#include "canvascontroller.h"
#include "canvassplitareacontroller.h"
#include "workspacecontroller.h"
#include "appcontroller.h"
#include "settingsmanager.h"

#include "workspaceview.h"

namespace PaintField
{

SidebarTabWidget::SidebarTabWidget(Workspace *workspace, QWidget *parent) :
    WorkspaceTabWidget(workspace, parent)
{
	setAutoDeletionEnabled(true);
}

bool SidebarTabWidget::tabIsInsertable(DockTabWidget *other, int index)
{
	Q_UNUSED(index)
	SidebarTabWidget *tabWidget = qobject_cast<SidebarTabWidget *>(other);
	return tabWidget && tabWidget->workspace() == workspace();
}

QObject *SidebarTabWidget::createNew()
{
	return new SidebarTabWidget(workspace(), 0);
}



bool WorkspaceMotherWidget::tabIsInsertable(DockTabWidget *src, int srcIndex)
{
	Q_UNUSED(srcIndex)
	
	SidebarTabWidget *tabWidget = qobject_cast<SidebarTabWidget *>(src);
	return tabWidget && tabWidget->workspace() == _workspace;
}



namespace MenuArranger
{

QMenu *createMenu(const QHash<QString, ActionInfo> &actionDeclarations, const QHash<QString, MenuInfo> &menuDeclarations, const QVariantMap &order);
QMenuBar *createMenuBar(const QHash<QString, ActionInfo> &actionDeclarations, const QHash<QString, MenuInfo> &menuDeclarations, const QVariant &order);
	
void associateMenuWithActions(QMenu *menu, const QActionList &actions);
void associateMenuBarWithActions(QMenuBar *menuBar, const QActionList &actions);

QMenu *createMenu(const QHash<QString, ActionInfo> &actionDeclarations, const QHash<QString, MenuInfo> &menuDeclarations, const QVariantMap &order)
{
	QString menuId = order["menu"].toString();
	if (menuId.isEmpty())
		return 0;
	
	QString menuTitle = menuDeclarations[menuId].text;
	
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
					ActionInfo actionInfo = actionDeclarations.value(id);
					
					if (actionInfo.text.isEmpty())
						actionInfo.text = id;
					
					WorkspaceMenuAction *action = new WorkspaceMenuAction(menu);
					action->setObjectName(id);
					action->setText(actionInfo.text);
					action->setShortcut(actionInfo.shortcut);
					menu->addAction(action);
				}
				break;
			}
			case QVariant::Map:
			{
				QMenu *childMenu = createMenu(actionDeclarations, menuDeclarations, child.toMap());
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

QMenuBar *createMenuBar(const QHash<QString, ActionInfo> &actionDeclarations, const QHash<QString, MenuInfo> &menuDeclarations, const QVariant &order)
{
	auto menuBar = new QMenuBar();
	
	for (const QVariant &menuOrder : order.toList())
		menuBar->addMenu(createMenu(actionDeclarations, menuDeclarations, menuOrder.toMap()));
	
	return menuBar;
}

void associateMenuWithActions(QMenu *menu, const QActionList &actions)
{
	for (QAction *action : menu->actions())
	{
		WorkspaceMenuAction *menuAction = qobject_cast<WorkspaceMenuAction *>(action);
		if (menuAction)
		{
			QAction *foundAction = findQObjectReverse(actions, menuAction->objectName());
			menuAction->setBackendAction(foundAction);
		}
		else
		{
			if (action->menu())
				associateMenuWithActions(action->menu(), actions);
		}
	}
}

void associateMenuBarWithActions(QMenuBar *menuBar, const QActionList &actions)
{
	for (QAction *action : menuBar->actions())
	{
		QMenu *menu = action->menu();
		if (menu)
			associateMenuWithActions(menu, actions);
	}
}

}

SideBarFrame::SideBarFrame(QWidget *parent) :
	QWidget(parent),
	_layout(new QVBoxLayout)
{
	_layout->setContentsMargins(0, 0, 0, 0);
	setLayout(_layout);
}

void SideBarFrame::setSideBar(QWidget *sideBar)
{
	if (_sideBar)
	{
		_layout->removeWidget(_sideBar);
		_sideBar->setParent(0);
		_sideBar->hide();
	}
	
	_sideBar = sideBar;
	_layout->addWidget(_sideBar);
	_sideBar->show();
}

WorkspaceMenuAction::WorkspaceMenuAction(QObject *parent) :
    QAction(parent)
{
	setEnabled(false);
}

void WorkspaceMenuAction::setBackendAction(QAction *action)
{
	if (action == _backendAction)
		return;
	
	if (_backendAction)
	{
		disconnect(_backendAction, 0, this, 0);
		disconnect(this, 0, _backendAction, 0);
	}
	
	_backendAction = action;
	setEnabled(action);
	
	if (action)
	{
		connect(action, SIGNAL(changed()), this, SLOT(onBackendActionChanged()));
		
		if (action->isCheckable())
		{
			setCheckable(true);
			connect(action, SIGNAL(triggered(bool)), this, SLOT(setChecked(bool)));
			connect(this, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		}
		else
		{
			connect(this, SIGNAL(triggered()), action, SLOT(trigger()));
		}
		onBackendActionChanged();
	}
}

void WorkspaceMenuAction::onBackendActionChanged()
{
	Q_ASSERT(_backendAction);
	setEnabled(_backendAction->isEnabled());
}


struct WorkspaceView::Data
{
	Workspace *workspace = 0;
	
	DockTabMotherWidget *motherWidget = 0;
	
	QList<SideBarFrame *> sideBarFrames;
	QList<QToolBar *> toolBars;
	
	Canvas *currentCanvas = 0;
};

WorkspaceView::WorkspaceView(Workspace *workspace, QWidget *parent) :
    QMainWindow(parent),
    d(new Data)
{
	d->workspace = workspace;
	d->currentCanvas = workspace->currentCanvas();
	
	setAnimated(false);
	
	d->motherWidget = new WorkspaceMotherWidget(workspace, 0);
	QMainWindow::setCentralWidget(d->motherWidget);
	
	{
		auto controller = new CanvasSplitAreaController(workspace, this);
		
		connect(workspace, SIGNAL(canvasShowRequested(Canvas*)),
		        controller, SLOT(addCanvas(Canvas*)));
		
		connect(workspace, SIGNAL(splitHorizontallyRequested()), controller, SLOT(splitCurrentHorizontally()));
		connect(workspace, SIGNAL(splitVerticallyRequested()), controller, SLOT(splitCurrentVertically()));
		connect(workspace, SIGNAL(closeCurrentSplitRequested()), controller, SLOT(closeCurrent()));
		
		d->motherWidget->setCentralWidget(controller->view());
	}
	
	connect(workspace, SIGNAL(currentCanvasChanged(Canvas*)), this, SLOT(setCurrentCanvas(Canvas*)));
	connect(this, SIGNAL(closeRequested()), workspace, SLOT(tryClose()));
	
	{
		QVariantMap workspaceItemOrderMap = appController()->settingsManager()->settings()["workspace-item-order"].toMap();
		
		createSideBarFrames(appController()->settingsManager()->sideBarInfoHash(),
		                    workspaceItemOrderMap["sidebars"]);
		createToolBars(appController()->settingsManager()->toolBarInfoHash(),
		               workspaceItemOrderMap["toolbars"]);
		createMenuBar(appController()->settingsManager()->actionInfoHash(),
		              appController()->settingsManager()->menuInfoHash(),
		              appController()->settingsManager()->settings()["menubar-order"]);
	}
	
	connect(workspace, SIGNAL(shouldBeDeleted(Workspace*)), this, SLOT(deleteLater()));
	connect(workspace, SIGNAL(focused()), this, SLOT(setFocus()));
	
	onCurrentCanvasPropertyChanged();
	
	updateWorkspaceItems();
	updateWorkspaceItemsForCanvas(workspace->currentCanvas());
	updateMenuBar();
}

void WorkspaceView::createSideBarFrames(const QHash<QString, SideBarInfo> &sideBarInfos, const QVariant &order)
{
	auto sideBarSplitterDirFromString = [](const QString &str)->DockTabMotherWidget::Direction
	{
		if (str == "left")
			return DockTabMotherWidget::Left;
		else if (str == "right")
			return DockTabMotherWidget::Right;
		else if (str == "top")
			return DockTabMotherWidget::Top;
		else if (str == "bottom")
			return DockTabMotherWidget::Bottom;
		else
			return DockTabMotherWidget::NoDirection;
	};
	
	QVariantMap orderMap = order.toMap();
	
	for (auto iter = orderMap.begin(); iter != orderMap.end(); ++iter)
	{
		DockTabMotherWidget::Direction splitterDir = sideBarSplitterDirFromString(iter.key());
		if (splitterDir != DockTabMotherWidget::NoDirection)
			createSideBarFramesInArea(splitterDir, sideBarInfos, iter.value());
	}
}

void WorkspaceView::createSideBarFramesInArea(DockTabMotherWidget::Direction splitterDir, const QHash<QString, SideBarInfo> &sidebarDeclarations, const QVariant &areaOrder)
{
	int i = 0;
	for (const QVariant &splitterOrder : areaOrder.toList())
		createSideBarFramesInSplitter(splitterDir, i++, sidebarDeclarations, splitterOrder.toList());
}

void WorkspaceView::createSideBarFramesInSplitter(DockTabMotherWidget::Direction splitterDir, int splitterIndex, const QHash<QString, SideBarInfo> &sidebarDeclarations, const QVariant &splitterOrder)
{
	for (const QVariant &tabWidgetOrder : splitterOrder.toList())
	{
		auto tabWidget = new SidebarTabWidget(workspace(), 0);
		
		for (const QString &sideBarName : tabWidgetOrder.toStringList())
		{
			if (sidebarDeclarations.contains(sideBarName))
			{
				auto sideBarFrame = new SideBarFrame;
				sideBarFrame->setObjectName(sideBarName);
				d->sideBarFrames << sideBarFrame;
				
				tabWidget->addTab(sideBarFrame, sidebarDeclarations[sideBarName].text);
			}
		}
		
		d->motherWidget->addTabWidget(tabWidget, splitterDir, splitterIndex);
	}
}

void WorkspaceView::createToolBars(const QHash<QString, ToolBarInfo> &toolBarInfos, const QVariant &order)
{
	auto toolBarAreaFromString = [](const QString &str)->Qt::ToolBarArea
	{
		if (str == "left")
			return Qt::LeftToolBarArea;
		else if (str == "right")
			return Qt::RightToolBarArea;
		else if (str == "top")
			return Qt::TopToolBarArea;
		else if (str == "bottom")
			return Qt::BottomToolBarArea;
		else
			return Qt::NoToolBarArea;
	};
	
	QVariantMap orderMap = order.toMap();
	
	for (auto iter = orderMap.begin(); iter != orderMap.end(); ++iter)
	{
		Qt::ToolBarArea area = toolBarAreaFromString(iter.key());
		if (area != Qt::NoToolBarArea)
		{
			createToolBarsInArea(area, toolBarInfos, iter->toList());
		}
	}
}

void WorkspaceView::createToolBarsInArea(Qt::ToolBarArea area, const QHash<QString, ToolBarInfo> &toolBarDeclarations, const QVariant &areaOrder)
{
	for (const QString &toolBarName : areaOrder.toStringList())
	{
		for (auto iter = toolBarDeclarations.begin(); iter != toolBarDeclarations.end(); ++iter)
		{
			if (iter.key() == toolBarName)
			{
				auto toolBar = new QToolBar(iter->text);
				toolBar->setObjectName(iter.key());
				addToolBar(area, toolBar);
				d->toolBars << toolBar;
				break;
			}
		}
	}
}

void WorkspaceView::createMenuBar(const QHash<QString, ActionInfo> &actionInfos, const QHash<QString, MenuInfo> &menuInfos, const QVariant &order)
{
	setMenuBar(MenuArranger::createMenuBar(actionInfos, menuInfos, order));
}

void WorkspaceView::setSidebar(const QString &id, QWidget *sidebar)
{
	applyMacSmallSize(sidebar);
	for (SideBarFrame *frame : d->sideBarFrames)
	{
		if (frame->objectName() == id)
		{
			frame->setSideBar(sidebar);
			return;
		}
	}
}

QToolBar *WorkspaceView::toolBar(const QString &id)
{
	for (auto toolBar : d->toolBars)
	{
		if (toolBar->objectName() == id)
			return toolBar;
	}
	return 0;
}

void WorkspaceView::associateMenuBarWithActions(const QActionList &actions)
{
	MenuArranger::associateMenuBarWithActions(menuBar(), actions);
}

Workspace *WorkspaceView::workspace()
{
	return d->workspace;
}

void WorkspaceView::setCurrentCanvas(Canvas *canvas)
{
	if (d->currentCanvas)
	{
		disconnect(d->currentCanvas->document(), 0, this, 0);
	}
	
	d->currentCanvas = canvas;
	
	if (d->currentCanvas)
	{
		connect(canvas->document(), SIGNAL(modifiedChanged(bool)), this, SLOT(onCurrentCanvasPropertyChanged()));
		connect(canvas->document(), SIGNAL(filePathChanged(QString)), this, SLOT(onCurrentCanvasPropertyChanged()));
	}
	
	updateWorkspaceItemsForCanvas(canvas);
	updateMenuBar();
	
	onCurrentCanvasPropertyChanged();
}

void WorkspaceView::onCurrentCanvasPropertyChanged()
{
	if (d->currentCanvas)
	{
		setWindowModified(d->currentCanvas->document()->isModified());
		setWindowFilePath(d->currentCanvas->document()->filePath());
		setWindowTitle(d->currentCanvas->document()->fileName() + "[*] - PaintField");
	}
	else
	{
		setWindowModified(false);
		setWindowTitle("PaintField");
	}
}

void WorkspaceView::updateWorkspaceItems()
{
	for (const QString &name : appController()->settingsManager()->sidebarNames())
	{
		QWidget *sidebar = sideBarForWorkspace(appController()->modules(), workspace()->modules(), name);
		if (sidebar)
			setSidebar(name, sidebar);
	}
	
	for (const QString &name : appController()->settingsManager()->toolbarNames())
	{
		QToolBar *toolBar = this->toolBar(name);
		if (toolBar)
			updateToolBar(appController()->modules(), workspace()->modules(), workspace()->currentCanvasModules(), toolBar, name);
	}
}

void WorkspaceView::updateWorkspaceItemsForCanvas(Canvas *canvas)
{
	Q_UNUSED(canvas)
	
	for (const QString &name : appController()->settingsManager()->sidebarNames())
	{
		QWidget *sidebar = sideBarForCanvas(workspace()->currentCanvasModules(), name);
		if (sidebar)
			setSidebar(name, sidebar);
	}
	
	for (const QString &name : appController()->settingsManager()->toolbarNames())
	{
		QToolBar *toolBar = this->toolBar(name);
		if (toolBar)
			updateToolBar(AppModuleList(), WorkspaceModuleList(), workspace()->currentCanvasModules(), toolBar, name);
	}
}

void WorkspaceView::updateMenuBar()
{
	QActionList actions = appController()->actions() + workspace()->actions() + workspace()->currentCanvasActions();
	associateMenuBarWithActions(actions);
}

void WorkspaceView::closeEvent(QCloseEvent *event)
{
	emit closeRequested();
	event->ignore();
}



}
