#include <QtGui>
#include "../debug.h"

#include "splittabareacontroller.h"

namespace PaintField
{




// SplitTabWidget

SplitTabWidget::SplitTabWidget(SplitTabAreaController *tabAreaController, QWidget *baseWindow, QWidget *parent) :
	FloatingDockTabWidget(baseWindow, parent),
	_tabAreaController(tabAreaController)
{
	commonInit();
}

SplitTabWidget::SplitTabWidget(SplitTabWidget *other, QWidget *parent) :
	FloatingDockTabWidget(other, parent),
	_tabAreaController(other->_tabAreaController)
{
	commonInit();
}

void SplitTabWidget::commonInit()
{
	setTabsClosable(true);
	_tabAreaController->registerTabWidget(this);
	connect(this, SIGNAL(tabClicked()), this, SLOT(notifyTabChange()));
	connect(this, SIGNAL(tabMovedIn()), this, SLOT(notifyTabChange()));
}

bool SplitTabWidget::tabIsInsertable(DockTabWidget *other, int index)
{
	Q_UNUSED(index)
	
	SplitTabWidget *tabWidget = qobject_cast<SplitTabWidget *>(other);
	
	return tabWidget && tabWidget->baseWindow() == baseWindow();
}

void SplitTabWidget::insertTab(int index, QWidget *widget, const QString &title)
{
	super::insertTab(index, widget, title);
}

QObject *SplitTabWidget::createNew()
{
	return new SplitTabWidget(this, 0);
}

void SplitTabWidget::notifyTabChange()
{
	_tabAreaController->setCurrentTabWidget(this);
}




// SplitTabDefaultWidget

SplitTabDefaultWidget::SplitTabDefaultWidget(SplitTabWidget *tabWidget, QWidget *parent) :
	QWidget(parent),
	_tabWidget(tabWidget)
{
	setAcceptDrops(true);
}

bool SplitTabDefaultWidget::dropDockTab(DockTabWidget *srcTabWidget, int srcIndex, const QPoint &pos)
{
	Q_UNUSED(pos)
	
	srcTabWidget->moveTab(srcIndex, _tabWidget, 0);
	return true;
}

bool SplitTabDefaultWidget::tabIsInsertable(DockTabWidget *src, int srcIndex)
{
	return _tabWidget->tabIsInsertable(src, srcIndex);
}

void SplitTabDefaultWidget::mousePressEvent(QMouseEvent *event)
{
	Q_UNUSED(event)
	emit clicked();
}





// SplitTabStackedWidget

SplitTabStackedWidget::SplitTabStackedWidget(SplitTabAreaController *tabAreaController, SplitTabWidget *tabWidget, QWidget *parent) :
	QStackedWidget(parent),
	_tabAreaController(tabAreaController),
	_tabWidget(tabWidget),
	_defaultWidget(new SplitTabDefaultWidget(tabWidget, 0))
{
	_tabWidget->setAutoDeletionEnabled(false);
	
	insertWidget(IndexTabWidget, tabWidget);
	insertWidget(IndexDefaultWidget, _defaultWidget);
	
	setCurrentIndex(IndexDefaultWidget);
	
	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabWidgetCurrentChanged(int)));
	connect(_defaultWidget, SIGNAL(clicked()), _tabWidget, SIGNAL(tabClicked()));
}

void SplitTabStackedWidget::onTabWidgetCurrentChanged(int index)
{
	if (index >= 0)
		setCurrentIndex(IndexTabWidget);
	else
		setCurrentIndex(IndexDefaultWidget);
}

QObject *SplitTabStackedWidget::createNew()
{
	auto newTabWidget = _tabWidget->createNewAs<SplitTabWidget>();
	return new SplitTabStackedWidget(_tabAreaController,  newTabWidget, 0);
}





// SplitTabAreaController

SplitTabAreaController::SplitTabAreaController(QWidget *baseWindow, QObject *parent) :
	QObject(parent)
{
	auto tabWidget = new SplitTabWidget(this, baseWindow, 0);
	auto stackedWidget = new SplitTabStackedWidget(this, tabWidget, 0);
	_rootSplit = new SplitAreaController(stackedWidget, 0);
	setCurrentSplit(_rootSplit);
}

void SplitTabAreaController::addTab(QWidget *tab, const QString &title)
{
	tabWidgetForSplit(_currentSplit)->addTab(tab, title);
}

void SplitTabAreaController::splitCurrentSplit(Qt::Orientation orientation)
{
	_currentSplit->insert(SplitAreaController::Second, orientation);
	setCurrentSplit(_currentSplit->childSplit(SplitAreaController::First));
}

void SplitTabAreaController::closeCurrentSplit()
{
	if (_currentSplit == _rootSplit)
		return;
	
	auto tabWidget = tabWidgetForCurrentSplit();
	
	tabWidget->requestCloseAllTabs();
	
	if (tabWidget->count())
		return;
	
	onTabWidgetAboutToBeDeleted(tabWidget);
	tabWidget->deleteLater();
	
	_currentSplit->parentSplit()->close(_currentSplit->index());
	setCurrentSplit(_currentSplit->parentSplit()->firstNonSplittedDescendant());
}

void SplitTabAreaController::setCurrentTabWidget(SplitTabWidget *tabWidget)
{
	Q_ASSERT(tabWidget == 0 ||  _tabWidgets.contains(tabWidget));
	
	if (_currentTabWidget != tabWidget)
	{
		if (_currentTabWidget  && _tabWidgets.contains(_currentTabWidget))
			disconnect(_currentTabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentTabWidgetCurrentChanged(int)));
		
		_currentTabWidget = tabWidget;
		
		SplitAreaController *split = splitForTabWidget(tabWidget);
		if (split)
			setCurrentSplit(split);
		
		if (tabWidget)
		{
			connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentTabWidgetCurrentChanged(int)));
			onCurrentTabWidgetCurrentChanged(tabWidget->currentIndex());
		}
		else
		{
			setCurrentTab(0);
		}
	}
}

void SplitTabAreaController::setCurrentTab(QWidget *tab)
{
	if (_currentTab != tab)
	{
		_currentTab = tab;
		
		for (SplitTabWidget *tabWidget : _tabWidgets)
		{
			if (tabWidget->contains(tab))
			{
				setCurrentTabWidget(tabWidget);
				tabWidget->setCurrentWidget(tab);
			}
		}
		
		emit currentTabChanged(tab);
	}
}

void SplitTabAreaController::removeTab(QWidget *tab)
{
	auto tabWidget = tabWidgetForTab(tab);
	if (tabWidget)
	{
		tabWidget->removeTab(tabWidget->indexOf(tab));
		tabWidget->deleteIfEmpty();
	}
}

void SplitTabAreaController::onCurrentTabWidgetCurrentChanged(int index)
{
	QWidget *tab = _currentTabWidget ? _currentTabWidget->widget(index) : 0;
	if (_currentTab != tab)
	{
		_currentTab = tab;
		emit currentTabChanged(tab);
	}
}

void SplitTabAreaController::onTabWidgetCloseRequested(int index)
{
	auto tabWidget = senderTabWidget();
	if (tabWidget)
		emit tabCloseRequested(tabWidget->widget(index));
}

void SplitTabAreaController::onTabWidgetCloseAllRequested()
{
	auto tabWidget = senderTabWidget();
	if (tabWidget)
		emit tabsCloseRequested(tabWidget->tabs());
}

void SplitTabAreaController::onTabWidgetAboutToBeDeleted(DockTabWidget *widget)
{
	SplitTabWidget *tabWidget = qobject_cast<SplitTabWidget *>(widget);
	if (tabWidget && _tabWidgets.contains(tabWidget))
	{
		_tabWidgets.removeAll(tabWidget);
		
		if (_currentTabWidget == tabWidget)
			setCurrentTabWidget(0);
	}
}

void SplitTabAreaController::setCurrentSplit(SplitAreaController *split)
{
	Q_ASSERT(split && split->widget());
	if (_currentSplit != split)
	{
		_currentSplit = split;
		setSplittable(_currentSplit == _rootSplit);
		setCurrentTabWidget(tabWidgetForCurrentSplit());
	}
}

void SplitTabAreaController::setSplittable(bool splittable)
{
	if (_isSplittable != splittable)
	{
		_isSplittable = splittable;
		emit splittableChanged(splittable);
	}
}

FloatingDockTabWidget *SplitTabAreaController::tabWidgetForTab(QWidget *tab)
{
	for (SplitTabWidget *tabWidget : _tabWidgets)
	{
		if (tabWidget->contains(tab))
			return tabWidget;
	}
	return 0;
}

SplitTabWidget *SplitTabAreaController::tabWidgetForSplit(SplitAreaController *split)
{
	SplitTabStackedWidget *stacked = qobject_cast<SplitTabStackedWidget *>(split->widget());
	Q_ASSERT(stacked);
	return stacked->tabWidget();
}

SplitAreaController *SplitTabAreaController::splitForWidget(QWidget *widget)
{
	return _rootSplit->findSplitForWidget(widget);
}

SplitAreaController *SplitTabAreaController::splitForTabWidget(SplitTabWidget *tabWidget)
{
	auto predicate = [tabWidget](SplitAreaController *splitCurrentSplit)->bool
	{
		SplitTabStackedWidget *stacked = qobject_cast<SplitTabStackedWidget *>(splitCurrentSplit->widget());
		return stacked && stacked->tabWidget() == tabWidget;
	};
	
	return _rootSplit->findSplit(predicate);
}

SplitTabWidget *SplitTabAreaController::senderTabWidget()
{
	SplitTabWidget *tabWidget = qobject_cast<SplitTabWidget *>(sender());
	if (tabWidget && _tabWidgets.contains(tabWidget))
		return tabWidget;
	else
		return 0;
}

void SplitTabAreaController::registerTabWidget(SplitTabWidget *widget)
{
	connect(widget, SIGNAL(willBeAutomaticallyDeleted(DockTabWidget*)), this, SLOT(onTabWidgetAboutToBeDeleted(DockTabWidget*)));
	connect(widget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabWidgetCloseRequested(int)));
	connect(widget, SIGNAL(closeAllTabsRequested()), this, SLOT(onTabWidgetCloseAllRequested()));
	_tabWidgets << widget;
}

}
