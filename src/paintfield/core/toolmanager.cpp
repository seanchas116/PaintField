#include <QActionGroup>
#include <QAction>

#include "appcontroller.h"
#include "extensionmanager.h"
#include "util.h"

#include "toolmanager.h"

namespace PaintField
{

ToolManager::ToolManager(QObject *parent) :
    QObject(parent),
	_actionGroup(new QActionGroup(this))
{
	createActions(appController()->settingsManager()->toolInfoHash());
	connect(_actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
}

void ToolManager::setCurrentTool(const QString &name)
{
	if (!_actionHash.values().contains(name))
		return;
	
	if (_currentTool == name)
		return;
	
	_currentTool = name;
	
	QAction *action = _actionHash.key(name);
	Q_ASSERT(action);
	if (!action->isChecked())
		action->setChecked(true);
	
	emit currentToolChanged(name);
}

void ToolManager::onActionTriggered(QAction *action)
{
	setCurrentTool(_actionHash[action]);
}

void ToolManager::createActions(const QHash<QString, ToolInfo> &infoHash)
{
	for (auto iter = infoHash.begin(); iter != infoHash.end(); ++iter)
	{
		auto action = new QAction(iter->icon, iter->text, this);
		action->setCheckable(true);
		action->setObjectName(iter.key());
		_actionGroup->addAction(action);
		_actionHash[action] = iter.key();
	}
}

}

