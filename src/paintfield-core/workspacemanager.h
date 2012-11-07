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
	void workspaceAboutToBeRemoved(WorkspaceController *controller);
	void currentWorkspaceChanged(WorkspaceController *controller);
	
public slots:
	
	/**
	 * Tries to close all workspaces.
	 * The app will quit if it is succeeded.
	 * @return 
	 */
	bool tryCloseAll();
	
	void newWorkspace();
	void setCurrentWorkspace(WorkspaceController *workspace);
	
	/**
	 * Removes a workspace.
	 * The app will quit if there is no workspace after it.
	 * @param workspace
	 */
	void removeWorkspace(WorkspaceController *workspace);
	
protected:
	
	void addWorkspace(WorkspaceController *workspace);
	
private slots:
	
	void onWorkspaceFocusIn();
	void onWorkspaceShouldBeDeleted(WorkspaceController *workspace);
	
private:
	
	QList<WorkspaceController *> _workspaces;
	WorkspaceController *_currentWorkspace = 0;
};

}

#endif // WORKSPACEMANAGER_H
