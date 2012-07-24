#include <QtGui>

#include "fsactionmanager.h"

QAction *FSActionManager::action(const QString &id)
{
	foreach (QAction *action, _actions)
	{
		if (action->objectName() == id)
			return action;
	}
	
	qWarning() << "FSActionManager::action: no action found.";
	return 0;
}

void FSActionManager::addAction(QAction *action)
{
	action->setParent(this);
	_actions << action;
}

void FSActionManager::addAction(QAction *action, const QString &id, const QString &text, const QKeySequence &shortcut)
{
	action->setObjectName(id);
	action->setText(text);
	action->setShortcut(shortcut);
	addAction(action);
}

void FSActionManager::connectTriggered(const QString &actionName, const QObject *receiver, const char *slot)
{
	QAction *action = this->action(actionName);
	Q_ASSERT(action);
	connect(action, SIGNAL(triggered()), receiver, slot);
}

void FSActionManager::connectToggled(const QString &actionName, const QObject *receiver, const char *slot)
{
	QAction *action = this->action(actionName);
	Q_ASSERT(action);
	connect(action, SIGNAL(toggled(bool)), receiver, slot);
}

void FSActionManager::connectToTrigger(const QObject *sender, const char *signal, const QString &actionName)
{
	QAction *action = this->action(actionName);
	Q_ASSERT(action);
	connect(sender, signal, action, SLOT(trigger()));
}

void FSActionManager::connectToSetEnabled(const QObject *sender, const char *signal, const QString &actionName)
{
	QAction *action = this->action(actionName);
	Q_ASSERT(action);
	connect(sender, signal, action, SLOT(setEnabled(bool)));
}


