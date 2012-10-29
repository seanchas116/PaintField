#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H

#include <QObject>
#include "workspacecontroller.h"

namespace PaintField
{

class WorkspaceManager : public QObject
{
	Q_OBJECT
public:
	explicit WorkspaceManager(QObject *parent = 0);
	
	WorkspaceController *currentWorkspace() { return _currentWorkspace; }
	QList<WorkspaceController *> workspaces() { return _workspaces; }
	
signals:
	
	void workspaceAdded(WorkspaceController *controller);
	void currentWorkspaceChanged(WorkspaceController *controller);
	
public slots:
	
	bool tryCloseAll();
	void newWorkspace();
	void setCurrentWorkspace(WorkspaceController *controller);
	
protected:
	
	void addWorkspace(WorkspaceController *controller);
	
private slots:
	
	void onWorkspaceFocusIn();
	
private:
	
	QList<WorkspaceController *> _workspaces;
	WorkspaceController *_currentWorkspace = 0;
};

}

#endif // WORKSPACEMANAGER_H
