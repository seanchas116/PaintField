#include <QVBoxLayout>

#include "tabwidget.h"

namespace PaintField
{

TabWidget::TabWidget(QWidget *parent) :
	TabWidget(new QTabBar, new QWidget, parent)
{}

TabWidget::TabWidget(QTabBar *tabBar, QWidget *defaultWidget, QWidget *parent) :
	QFrame(parent),
	_tabBar(tabBar),
	_defaultWidget(defaultWidget),
	_currentIndex(-1),
	_currentWidget(0),
	_isDefaultWidgetShown(true),
	_layout(new QVBoxLayout)
{
	Q_ASSERT(tabBar);
	Q_ASSERT(defaultWidget);
	
	connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(setCurrentIndex(int)));
	
	_layout->addWidget(_defaultWidget);
	_layout->setSpacing(0);
	_layout->setContentsMargins(0, 0, 0, 0);
	setLayout(_layout);
}

void TabWidget::insertTab(int index, QWidget *widget, const QString &label)
{
	Q_ASSERT(contains(index) || index == count());
	Q_ASSERT(widget);
	
	_widgets.insert(index, widget);
	_tabBar->insertTab(index, label);
	
	if (_currentIndex == -1)
		_currentIndex = 0;
	
	updateDisplayWidget();
}

QWidget *TabWidget::takeTab(int index)
{
	Q_ASSERT(contains(index));
	
	QWidget *widget = _widgets.takeAt(index);
	
	if (_widgets.size() == 0)
		_currentIndex = -1;
	
	if (index < _currentIndex)
		_currentIndex--;
	
	_tabBar->removeTab(index);
	
	updateDisplayWidget();
	
	emit currentIndexChanged(_currentIndex);
	
	return widget;
}

void TabWidget::takeTab(QWidget *widget)
{
	int index = indexOf(widget);
	if (index >= 0)
		takeTab(index);
}

void TabWidget::setCurrentIndex(int index)
{
	Q_ASSERT(contains(index) || index == -1);
	
	if (_currentIndex != index)
	{
		_currentIndex = index;
		updateDisplayWidget();
		_tabBar->setCurrentIndex(index);
	}
}

void TabWidget::setCurrentWidget(QWidget *widget)
{
	int index = indexOf(widget);
	if (index >= 0)
		setCurrentIndex(index);
}

void TabWidget::onWidgetDeleted(QObject *obj)
{
	takeTab(qobject_cast<QWidget *>(obj));
}

void TabWidget::addWidgetToLayout(QWidget *widget)
{
	_layout->addWidget(widget);
	widget->setVisible(true);
}

void TabWidget::removeWidgetFromLayout(QWidget *widget)
{
	widget->setVisible(false);
	_layout->removeWidget(widget);
	widget->setParent(0);
}

void TabWidget::updateDisplayWidget()
{
	if (_isDefaultWidgetShown && _currentIndex >= 0)
	{
		removeWidgetFromLayout(_defaultWidget);
		
		addWidgetToLayout(_tabBar);
		
		_currentWidget = _widgets.at(_currentIndex);
		addWidgetToLayout(_currentWidget);
		_isDefaultWidgetShown = false;
		return;
	}
	if (_currentIndex == -1)
	{
		removeWidgetFromLayout(_tabBar);
		removeWidgetFromLayout(_currentWidget);
		
		addWidgetToLayout(_defaultWidget);
		_isDefaultWidgetShown = true;
		_currentWidget = 0;
		return;
	}
	
	removeWidgetFromLayout(_currentWidget);
	
	_currentWidget = _widgets.at(_currentIndex);
	addWidgetToLayout(_currentWidget);
}

}
