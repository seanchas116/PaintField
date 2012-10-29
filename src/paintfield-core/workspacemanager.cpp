#include "application.h"
#include "modulemanager.h"

#include "workspacemanager.h"

namespace PaintField
{

WorkspaceManager::WorkspaceManager(QObject *parent) :
	QObject(parent)
{
}

bool WorkspaceManager::tryCloseAll()
{
	for (WorkspaceController *controller : _workspaces)
	{
		if (!controller->tryClose())
			return false;
	}
	return true;
}

void WorkspaceManager::newWorkspace()
{
	addWorkspace(new WorkspaceController(this));
}

void WorkspaceManager::setCurrentWorkspace(WorkspaceController *workspace)
{
	if (_workspaces.contains(workspace))
	{
		if (_currentWorkspace != workspace)
		{
			_currentWorkspace = workspace;
			workspace->setFocus();
			emit currentWorkspaceChanged(workspace);
		}
	}
}

void WorkspaceManager::addWorkspace(WorkspaceController *workspace)
{
	_workspaces << workspace;
	
	connect(workspace, SIGNAL(focused()), this, SLOT(onWorkspaceFocusIn()));
	
	workspace->addModules(app()->moduleManager()->createWorkspaceModules(workspace, workspace));
	workspace->addNullCanvasModules(app()->moduleManager()->createCanvasModules(0, workspace));
	
	emit workspaceAdded(workspace);
	
	workspace->createView()->show();
	setCurrentWorkspace(workspace);
}

void WorkspaceManager::onWorkspaceFocusIn()
{
	WorkspaceController *workspace = qobject_cast<WorkspaceController *>(sender());
	if (workspace)
		setCurrentWorkspace(workspace);
}

}
