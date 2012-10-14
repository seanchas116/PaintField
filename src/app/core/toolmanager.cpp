#include <QtGui>

#include "application.h"
#include "toolmanager.h"

namespace PaintField
{

ToolManager::ToolManager(QObject *parent) :
    QObject(parent),
	_actionGroup(new QActionGroup(this))
{
	_toolFactoryList = app()->toolFactories();
	createActions();
	connect(_actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
}

ToolFactory *ToolManager::findToolFactory(const QString &name)
{
	foreach (ToolFactory *factory, _toolFactoryList) {
		if (factory->toolName() == name)
			return  factory;
	}
	return 0;
}

void ToolManager::setCurrentToolFactory(const QString &name)
{
	_currentToolFactory = findToolFactory(name);
	
	if (!_toolFactoryList.contains(_currentToolFactory))
	{
		qWarning() << Q_FUNC_INFO << ": no such factory found.";
		return;
	}
	
	int index = _toolFactoryList.indexOf(_currentToolFactory);
	_actionList.at(index)->trigger();
	
	emit currentToolFactoryChanged(_currentToolFactory);
}

void ToolManager::actionTriggered(QAction *action)
{
	Q_ASSERT(_actionList.contains(action));
	int index = _actionList.indexOf(action);
	_currentToolFactory = _toolFactoryList.at(index);
	
	emit currentToolFactoryChanged(_currentToolFactory);
}

void ToolManager::createActions()
{
	foreach (ToolFactory *factory, _toolFactoryList)
	{
		QAction *action = new QAction(factory->icon(), factory->text(), this);
		action->setCheckable(true);
		action->setObjectName(factory->toolName());
		_actionGroup->addAction(action);
		_actionList << action;
	}
}

}

