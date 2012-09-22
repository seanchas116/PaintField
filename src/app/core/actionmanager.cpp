#include <QtGui>

#include "actionmanager.h"

namespace PaintField
{

QAction *ActionManager::action(const QString &id)
{
	if (_actionHash.contains(id))
	{
		return _actionHash[id].action;
	}
	
	qWarning() << Q_FUNC_INFO << ": action not found";
	return 0;
}

QAction *ActionManager::addAction(QAction *action, const QString &id)
{
	_actionHash[id].action = action;
	connect(action, SIGNAL(destroyed()), this, SLOT(onActionDestroyed()));
	return action;
}

QAction *ActionManager::addAction(QAction *action, const QString &id, const QObject *receiver, const char *triggeredSlot)
{
	addAction(action, id);
	connect(action, SIGNAL(triggered()), receiver, triggeredSlot);
	return action;
}

void ActionManager::removeAction(const QString &id)
{
	if (_actionHash[id].action)
		disconnect(_actionHash[id].action, 0, this, 0);
	
	clearAction(id);
}

void ActionManager::removeAction(QAction *action)
{
	disconnect(action, 0, this, 0);
	clearAction(action);
}

void ActionManager::declareAction(const QString &id, const QString &text, const QKeySequence &defaultShortcut)
{
	_actionHash[id].text = text;
	_actionHash[id].shortcut = defaultShortcut;
}

void ActionManager::declareMenu(const QString &id, const QString &text)
{
	_menuHash[id] = text;
}

void ActionManager::connectAction(const QString &id, const char *signal, const QObject *receiver, const char *slot)
{
	QAction *action = this->action(id);
	
	if (!action)
		return;
	
	connect(action, signal, receiver, slot);
}

void ActionManager::connectAction(const QObject *sender, const char *signal, const QString &id, const char *slot)
{
	QAction *action = this->action(id);
	
	if (!action)
		return;
	
	connect(sender, signal, action, slot);
}

void ActionManager::arrangeMenu(QMenu *menu, const QVariantMap &order)
{
	menu->setTitle(_menuHash[order["title"].toString()]);
	
	QVariantList children = order["children"].toList();
	
	foreach (const QVariant &child, children)
	{
		QVariantMap subOrder = child.toMap();
		
		if (subOrder.size()) // child is menu
		{
			QMenu *subMenu = new QMenu();
			arrangeMenu(subMenu, subOrder);
			menu->addMenu(subMenu);
			continue;
		}
		
		QString id = child.toString();
		
		if (!id.isEmpty())	// child is action
		{
			ActionItem item = _actionHash[id];
			
			if (item.action)
			{
				menu->addAction(item.action);
			}
			else
			{
				QAction *action = new QAction(this);
				action->setText(item.text);
				action->setIcon(item.icon);
				action->setShortcut(item.shortcut);
				menu->addAction(action);
			}
			continue;
		}
		
		menu->addSeparator();	// child is not an string nor an object
	}
}


void ActionManager::onActionDestroyed()
{
	QAction *destroyedAction = qobject_cast<QAction *>(sender());
	
	if (destroyedAction)
		clearAction(destroyedAction);
}

void ActionManager::clearAction(const QString &id)
{
	_actionHash[id].action = 0;
}

void ActionManager::clearAction(QAction *action)
{
	for (auto iter = _actionHash.begin(); iter != _actionHash.end(); ++iter)
	{
		if (iter->action == action)
			iter->action = 0;
	}
}

}
