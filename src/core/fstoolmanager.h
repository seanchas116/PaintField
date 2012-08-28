#ifndef FSTOOLMANAGER_H
#define FSTOOLMANAGER_H

#include <QObject>
#include "fstool.h"

class QActionGroup;

class FSToolManager : public QObject
{
	Q_OBJECT
	
public:
	
	explicit FSToolManager(QObject *parent = 0);
	
	void addToolFactory(FSToolFactory *factory);
	
	FSToolFactory *findToolFactory(const QString &name);
	QList<QString> toolNameList() const { return _toolNameList; }
	QList<FSToolFactory *> toolFactoryList() { return _toolFactoryList; }
	QList<QAction *> actionList() { return _actionList; }
	
	FSToolFactory *currentToolFactory() { return _currentToolFactory; }
	void setCurrentToolFactory(const QString &name);
	
signals:
	
	void currentToolFactoryChanged(FSToolFactory *factory);
	
public slots:
	
private slots:
	
	void actionTriggered(QAction *action);
	
private:
	
	QList<QString> _toolNameList;
	QList<FSToolFactory *> _toolFactoryList;
	QList<QAction *> _actionList;
	QActionGroup *_actionGroup;
	
	FSToolFactory *_currentToolFactory;
};

#endif // FSTOOLMANAGER_H
