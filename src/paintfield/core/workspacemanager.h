#pragma once

#include <QObject>
#include "workspace.h"

namespace PaintField
{

class WorkspaceManager : public QObject
{
	Q_OBJECT
public:
	explicit WorkspaceManager(QObject *parent = 0);
	
	Workspace *currentWorkspace() { return _currentWorkspace; }
	QList<Workspace *> workspaces() { return _workspaces; }
	
signals:
	
	void workspaceAdded(Workspace *controller);
	void workspaceAboutToBeRemoved(Workspace *controller);
	void currentWorkspaceChanged(Workspace *controller);
	
public slots:
	
	void newWorkspace();
	void setCurrentWorkspace(Workspace *workspace);
	
	/**
	 * Tries to close all workspaces.
	 * The app will quit if it is succeeded.
	 * @return 
	 */
	void closeAllAndQuit();
	
	/**
	 * Closes a workspace.
	 * The app will quit if there is no workspace after it.
	 * @param workspace
	 */
	void closeWorkspace(Workspace *workspace);
	
	void loadLastWorkspaces();
	
protected:
	
	void addWorkspace(Workspace *workspace);
	
private slots:
	
	void onWorkspaceFocusIn();
	void onWorkspaceShouldBeDeleted(Workspace *workspace);
	void onFocusWidgetChanged(QWidget *old, QWidget *now);
	
private:
	
	
	void removeWorkspace(Workspace *workspace);
	
	QList<Workspace *> _workspaces;
	Workspace *_currentWorkspace = 0;
};

}

