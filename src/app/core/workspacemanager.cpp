#include "workspacemanager.h"

namespace PaintField
{

WorkspaceManager::WorkspaceManager(QObject *parent) :
	QObject(parent)
{
}

void WorkspaceManager::newWorkspace()
{
	addWorkspace(new WorkspaceController(this));
}

void WorkspaceManager::setCurrentWorkspace(WorkspaceController *controller)
{
	if (_workspaces.contains(controller))
	{
		if (_currentWorkspace != controller)
		{
			_currentWorkspace = controller;
			controller->setFocus();
			emit currentWorkspaceChanged(controller);
		}
	}
}

void WorkspaceManager::addWorkspace(WorkspaceController *controller)
{
	connect(controller, SIGNAL(focused()), this, SLOT(onWorkspaceFocusIn()));
	
	_workspaces << controller;
	emit workspaceAdded(controller);
	controller->show();
	setCurrentWorkspace(controller);
}

void WorkspaceManager::onWorkspaceFocusIn()
{
	WorkspaceController *workspace = qobject_cast<WorkspaceController *>(sender());
	if (workspace)
		setCurrentWorkspace(workspace);
}

}
