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
	
	void addToolFactory(ToolFactory *factory);
	
	ToolFactory *findToolFactory(const QString &name);
	QList<QString> toolNameList() const { return _toolNameList; }
	QList<ToolFactory *> toolFactoryList() { return _toolFactoryList; }
	QList<QAction *> actionList() { return _actionList; }
	
	ToolFactory *currentToolFactory() { return _currentToolFactory; }
	void setCurrentToolFactory(const QString &name);
	
signals:
	
	void currentToolFactoryChanged(ToolFactory *factory);
	
public slots:
	
private slots:
	
	void actionTriggered(QAction *action);
	
private:
	
	QList<QString> _toolNameList;
	QList<ToolFactory *> _toolFactoryList;
	QList<QAction *> _actionList;
	QActionGroup *_actionGroup;
	
	ToolFactory *_currentToolFactory;
};

}

#endif // FSTOOLMANAGER_H
