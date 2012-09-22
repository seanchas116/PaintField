#include <QtGui>
#include "toolmanager.h"

namespace PaintField
{

ToolManager::ToolManager(QObject *parent) :
    QObject(parent),
	_actionGroup(new QActionGroup(this)),
	_currentToolFactory(0)
{
	connect(_actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
}

void ToolManager::addToolFactory(ToolFactory *factory)
{
	if (!factory)
	{
		qWarning() << __PRETTY_FUNCTION__ << ": null factory";
		return;
	}
	
	if (!_currentToolFactory)
		_currentToolFactory = factory;
	
	factory->setParent(this);
	_toolFactoryList << factory;
	_toolNameList << factory->toolName();
	
	QAction *action = new QAction(factory->icon(), factory->text(), this);
	action->setCheckable(true);
	action->setObjectName(factory->toolName());
	_actionGroup->addAction(action);
	_actionList << action;
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
		qWarning() << __PRETTY_FUNCTION__ << ": no such factory found.";
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

}

