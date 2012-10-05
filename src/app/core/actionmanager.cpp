#include "actionmanager.h"

namespace PaintField
{

ActionManager::ActionManager(QObject *parent) :
	QObject(parent)
{
}

QAction *ActionManager::addAction(const QString &id, const QString &text, const QKeySequence &shortcut)
{
	QAction *action = new QAction(this);
	action->setObjectName(id);
	action->setText(text);
	action->setShortcut(shortcut);
	
	addAction(action);
	return action;
}

QAction *ActionManager::actionForId(const QString &id)
{
	QListIterator<QAction *> iter(_actions);
	iter.toBack();
	while (iter.hasPrevious())
	{
		QAction *action = iter.previous();
		if (action->objectName() == id)
			return action;
	}
	return 0;
}

}
