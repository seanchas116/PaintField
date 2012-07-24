#include <QtGui>

#include "fscore.h"
#include "fstoolmanager.h"

FSToolManager::FSToolManager(QObject *parent) :
    QObject(parent),
	_actionGroup(new QActionGroup(this)),
	_currentToolFactory(0)
{
	connect(_actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
}

void FSToolManager::addToolFactory(FSToolFactory *factory)
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
	action->setObjectName(factory->toolName());
	_actionGroup->addAction(action);
	_actionList << action;
}

FSToolFactory *FSToolManager::findToolFactory(const QString &name)
{
	foreach (FSToolFactory *factory, _toolFactoryList) {
		if (factory->toolName() == name)
			return  factory;
	}
	return 0;
}

void FSToolManager::setCurrentToolFactory(const QString &name)
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

void FSToolManager::actionTriggered(QAction *action)
{
	Q_ASSERT(_actionList.contains(action));
	int index = _actionList.indexOf(action);
	_currentToolFactory = _toolFactoryList.at(index);
	
	emit currentToolFactoryChanged(_currentToolFactory);
}

