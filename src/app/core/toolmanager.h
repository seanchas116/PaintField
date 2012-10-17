#ifndef FSTOOLMANAGER_H
#define FSTOOLMANAGER_H

#include <QObject>
#include "tool.h"

class QActionGroup;

namespace PaintField
{

class ToolManager : public QObject
{
	Q_OBJECT
	
public:
	
	explicit ToolManager(QObject *parent = 0);
	
	QList<ToolFactory *> toolFactories() { return _toolFactoryList; }
	QList<QAction *> actions() { return _actionList; }
	
	ToolFactory *currentToolFactory() { return _currentToolFactory; }
	void setCurrentToolFactory(const QString &name);
	
signals:
	
	void currentToolFactoryChanged(ToolFactory *factory);
	
public slots:
	
private slots:
	
	void actionTriggered(QAction *action);
	
private:
	
	void createActions();
	
	QList<ToolFactory *> _toolFactoryList;
	QList<QAction *> _actionList;
	QActionGroup *_actionGroup = 0;
	
	ToolFactory *_currentToolFactory = 0;
};

}

#endif // FSTOOLMANAGER_H
