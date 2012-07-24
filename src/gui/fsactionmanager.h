#ifndef FSACTIONMANAGER_H
#define FSACTIONMANAGER_H

#include <QObject>
#include <QAction>

class FSActionManager : public QObject
{
	Q_OBJECT
public:
	
	/**
	  This enum describes the types used in FSActionManager.
	*/
	enum ActionType
	{
		ActionTypeNormal,
		/** Actions which targets documents / canvas views. Disabled when no file is opened. */
		ActionTypeCanvasView,
		/** Actions which targets layers. Disabled when no layer is selected. */
		ActionTypeLayer,
		/** Edit actions (copy, paste, ...). Routed and enabled/disabled automatically when the focus widget is changed. */
		ActionTypeEdit
	};
	
	explicit FSActionManager(QObject *parent = 0) : QObject(parent) {}
	
	/**
	  Finds the action that has the object name id.
	  If no action is found, It returns 0.
	*/
	QAction *action(const QString &id);
	
	/**
	  Adds an action. The FSActionManager takes its ownership.
	*/
	void addAction(QAction *action);
	void addAction(QAction *action, const QString &id, const QString &text, const QKeySequence &shortcut = QKeySequence());
	
	void connectTriggered(const QString &actionName, const QObject *receiver, const char *slot);
	void connectToggled(const QString &actionName, const QObject *receiver, const char *slot);
	
	void connectToTrigger(const QObject *sender, const char *signal, const QString &actionName);
	void connectToSetEnabled(const QObject *sender, const char *signal, const QString &actionName);
	
	/**
	  Finds the action that has the object name id.
	  If no action is found, It returns 0.
	*/
	QAction *operator[](const QString &actionName) { return action(actionName); }
	
private:
	
	QList<QAction *> _actions;
};

#endif // FSACTIONMANAGER_H
