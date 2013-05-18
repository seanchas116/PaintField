#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QCloseEvent>

#include "proxyaction.h"
#include "widgets/docktabmotherwidget.h"
#include "canvas.h"
#include "canvassplitareacontroller.h"
#include "workspace.h"
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
					
					auto action = new ProxyAction(menu);
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
		auto menuAction = qobject_cast<ProxyAction *>(action);
		if (menuAction)
		{
			QAction *foundAction = Util::findQObjectReverse(actions, menuAction->objectName());
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

struct WorkspaceView::Data
{
	Workspace *workspace = 0;
	
	DockTabMotherWidget *motherWidget = 0;
	
	QList<SideBarFrame *> sideBarFrames;
	QList<QToolBar *> toolBars;
	
	Canvas *currentCanvas = 0;
};

WorkspaceView::WorkspaceView(Workspace *workspace, const QVariantMap &state, QWidget *parent) :
    QMainWindow(parent),
    d(new Data)
{
	workspace->setView(this);
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
	connect(workspace, SIGNAL(currentCanvasDocumentPropertyChanged()), this, SLOT(onCurrentCanvasDocumentPropertyChanged()));
	
	// load state settings
	{
		auto settingsManager = appController()->settingsManager();
		
		{
			auto workspaceItemOrderMap = state["item-order"].toMap();
			
			createSideBarFrames(settingsManager->sideBarInfoHash(),
			                    workspaceItemOrderMap["sidebars"]);
			createToolBars(settingsManager->toolBarInfoHash(),
			               workspaceItemOrderMap["toolbars"]);
			createMenuBar(settingsManager->actionInfoHash(),
			              settingsManager->menuInfoHash(),
			              settingsManager->value({"menubar-order"}));
		}
		
		d->motherWidget->setSizesState(state["sidebar-sizes"].toMap());
		d->motherWidget->setTabIndexState(state["sidebar-tab-indexes"].toMap());
	}
	
	connect(workspace, SIGNAL(shouldBeDeleted(Workspace*)), this, SLOT(deleteLater()));
	connect(workspace, SIGNAL(focused()), this, SLOT(setFocus()));
	
	onCurrentCanvasDocumentPropertyChanged();
	
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
	Util::applyMacSmallSize(sidebar);
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

QVariantMap WorkspaceView::saveState() const
{
	QVariantMap map;
	map["sidebar-sizes"] = d->motherWidget->sizesState();
	map["sidebar-tab-indexes"] = d->motherWidget->tabIndexState();
	
	{
		QVariantMap itemOrder;
		
		{
			QVariantMap toolBarOrder;
			QHash<Qt::ToolBarArea, QVariantList> toolBarHash;
			
			for (auto toolBar : d->toolBars)
				toolBarHash[toolBarArea(toolBar)] << toolBar->objectName();
			
			auto areaName = [](Qt::ToolBarArea area)
			{
				switch (area)
				{
					default:
					case Qt::TopToolBarArea:
						return "top";
					case Qt::BottomToolBarArea:
						return "bottom";
					case Qt::LeftToolBarArea:
						return "left";
					case Qt::RightToolBarArea:
						return "right";
				};
			};
			
			for (auto it = toolBarHash.begin(); it != toolBarHash.end(); ++it)
				toolBarOrder[areaName(it.key())] = it.value();
			
			itemOrder["toolbars"] = toolBarOrder;
		}
		
		itemOrder["sidebars"] = d->motherWidget->tabObjectNameState();
		
		map["item-order"] = itemOrder;
	}
	
	return map;
}

void WorkspaceView::setCurrentCanvas(Canvas *canvas)
{
	d->currentCanvas = canvas;
	
	updateWorkspaceItemsForCanvas(canvas);
	updateMenuBar();
	
	onCurrentCanvasDocumentPropertyChanged();
}

void WorkspaceView::onCurrentCanvasDocumentPropertyChanged()
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
		QWidget *sidebar = ExtensionUtil::sideBarForWorkspace(appController()->extensions(), workspace()->extensions(), name);
		if (sidebar)
			setSidebar(name, sidebar);
	}
	
	for (const QString &name : appController()->settingsManager()->toolbarNames())
	{
		QToolBar *toolBar = this->toolBar(name);
		if (toolBar)
			ExtensionUtil::updateToolBar(appController()->extensions(), workspace()->extensions(), workspace()->currentCanvasModules(), toolBar, name);
	}
}

void WorkspaceView::updateWorkspaceItemsForCanvas(Canvas *canvas)
{
	Q_UNUSED(canvas)
	
	for (const QString &name : appController()->settingsManager()->sidebarNames())
	{
		QWidget *sidebar = ExtensionUtil::sideBarForCanvas(workspace()->currentCanvasModules(), name);
		if (sidebar)
			setSidebar(name, sidebar);
	}
	
	for (const QString &name : appController()->settingsManager()->toolbarNames())
	{
		QToolBar *toolBar = this->toolBar(name);
		if (toolBar)
			ExtensionUtil::updateToolBar(AppExtensionList(), WorkspaceExtensionList(), workspace()->currentCanvasModules(), toolBar, name);
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
