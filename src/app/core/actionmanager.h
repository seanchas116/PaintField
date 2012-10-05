#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QAction>

namespace PaintField
{

/**
 * The ActionManager class provides the 
 */
class ActionManager : public QObject
{
	Q_OBJECT
public:
	explicit ActionManager(QObject *parent = 0);
	
	/**
	 * Adds an action to the action manager.
	 * The action manager takes the action's ownership.
	 * @param action
	 */
	void addAction(QAction *action)
	{
		action->setParent(this);
		_actions << action;
	}
	
	QAction *addAction(const QString &id, const QString &text, const QKeySequence &shortcut = QKeySequence());
	QAction *addAction(const QString &id, QObject *receiver, const char *onTriggeredSlot, const QString &text, const QKeySequence &shortcut = QKeySequence())
	{
		QAction *action = addAction(id, text, shortcut);
		connect(action, SIGNAL(triggered()), receiver, onTriggeredSlot);
		return action;
	}
	
	/**
	 * Searches for an action.
	 * @param id The action's object name
	 * @return The found action
	 */
	QAction *actionForId(const QString &id);
	
signals:
	
public slots:
	
private:
	
	QList<QAction *> _actions;
};

}

#endif // ACTIONMANAGER_H
