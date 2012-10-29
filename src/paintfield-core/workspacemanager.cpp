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
	controller->addModules(app()->moduleManager()->createWorkspaceModules(controller));
	controller->createView()->show();
	setCurrentWorkspace(controller);
}

void WorkspaceManager::onWorkspaceFocusIn()
{
	WorkspaceController *workspace = qobject_cast<WorkspaceController *>(sender());
	if (workspace)
		setCurrentWorkspace(workspace);
}

}
