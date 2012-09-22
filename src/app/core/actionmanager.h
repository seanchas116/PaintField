#ifndef FSACTIONMANAGER_H
#define FSACTIONMANAGER_H

#include <QObject>
#include <QAction>
#include <QStandardItemModel>


namespace PaintField {

class ActionManager : public QObject
{
	Q_OBJECT
public:
	
	struct ActionItem
	{
		ActionItem() : action(0) {}
		
		QString text;
		QKeySequence shortcut;
		QIcon icon;
		QAction *action;
	};
	
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
	
	explicit ActionManager(QObject *parent = 0) : QObject(parent) {}
	
	/**
	  Finds the action that has the object name id.
	  If no action is found, It returns 0.
	*/
	QAction *action(const QString &id);
	
	/**
	  Declares an action. It does not add the action.
	  Action declaration will 
	*/
	void declareAction(const QString &id, const QString &text, const QKeySequence &defaultShortcut = QKeySequence());
	
	void declareMenu(const QString &id, const QString &text);
	
	/**
	  Adds an action. The FSActionManager will not take its ownership.
	  @return added action
	*/
	QAction *addAction(QAction *action, const QString &id);
	
	QAction *addAction(QAction *action, const QString &id, const QObject *receiver, const char *triggeredSlot);
	
	/**
	  Removes an action.
	*/
	void removeAction(QAction *action);
	void removeAction(const QString &id);
	
	/**
	  Connect an action's 
	*/
	void connectAction(const QString &id, const char *signal, const QObject *receiver, const char *slot);
	void connectAction(const QObject *sender, const char *signal, const QString &id, const char *slot);
	
	void connectToTrigger(const QObject *sender, const char *signal, const QString &id)
	{
		connectAction(sender, signal, id, SLOT(trigger()));
	}
	
	void connectToSetEnabled(const QObject *sender, const char *signal, const QString &id)
	{
		connectAction(sender, signal, id, SLOT(setEnabled(bool)));
	}
	
	void connectTriggered(const QString &id, const QObject *receiver, const char *slot)
	{
		connectAction(id, SIGNAL(triggered()), receiver, slot);
	}
	
	/**
	  Arranges the menu from the declared actions.
	  If the action is declared but not added, a dummy action (disabled) is added to the menu.
	*/
	void arrangeMenu(QMenu *menu, const QVariantMap &order);
	
	/**
	  Finds the action that has the object name id.
	  If no action is found, It returns 0.
	*/
	QAction *operator[](const QString &actionName) { return action(actionName); }
	
private slots:
	
	void onActionDestroyed();
	
private:
	
	void clearAction(const QString &id);
	void clearAction(QAction *action);
	
	QHash<QString, ActionItem> _actionHash;
	QHash<QString, QString> _menuHash;
};

}

#endif // FSACTIONMANAGER_H
