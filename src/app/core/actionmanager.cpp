#include "actionmanager.h"

namespace PaintField
{

ActionManager::ActionManager(QObject *parent) :
	QObject(parent)
{
}

QAction *ActionManager::addAction(const QString &id)
{
	QAction *action = new QAction(this);
	action->setObjectName(id);
	addAction(action);
	return action;
}

}
