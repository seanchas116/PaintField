#ifndef FSTOOLMANAGER_H
#define FSTOOLMANAGER_H

#include <QObject>
#include "application.h"

class QActionGroup;

namespace PaintField
{

class ToolManager : public QObject
{
	Q_OBJECT
	
public:
	
	explicit ToolManager(QObject *parent = 0);
	
	QList<QAction *> actions() { return _actionHash.keys(); }
	
	QString currentTool() const { return _currentTool; }
	void setCurrentTool(const QString &name);
	
signals:
	
	void currentToolChanged(const QString &name);
	
public slots:
	
private slots:
	
	void onActionTriggered(QAction *action);
	
private:
	
	void createActions(const ToolInfoHash &infoHash);
	
	QString _currentTool;
	QHash<QAction *, QString> _actionHash;
	QActionGroup *_actionGroup = 0;
};

}

#endif // FSTOOLMANAGER_H
