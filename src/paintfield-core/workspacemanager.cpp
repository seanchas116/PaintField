#include <QDesktopWidget>
#include "application.h"
#include "appcontroller.h"
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

void WorkspaceManager::removeWorkspace(WorkspaceController *workspace)
{
	if (_workspaces.contains(workspace))
	{
		if (_workspaces.size() == 1)
			qApp->quit();
		
		if (_currentWorkspace == workspace)
			_currentWorkspace = 0;
		
		_workspaces.removeAll(workspace);
		emit workspaceAboutToBeRemoved(workspace);
		workspace->deleteLater();
	}
}

void WorkspaceManager::addWorkspace(WorkspaceController *workspace)
{
	_workspaces << workspace;
	
	connect(workspace, SIGNAL(focused()), this, SLOT(onWorkspaceFocusIn()));
	connect(workspace, SIGNAL(shouldBeDeleted(WorkspaceController*)), this, SLOT(removeWorkspace(WorkspaceController*)));
	
	workspace->addModules(appController()->moduleManager()->createWorkspaceModules(workspace, workspace));
	workspace->addNullCanvasModules(appController()->moduleManager()->createCanvasModules(0, workspace));
	
	emit workspaceAdded(workspace);
	
	maximizeWindowSize(workspace->view());
	
	workspace->updateView();
	workspace->view()->show();
	
	setCurrentWorkspace(workspace);
}

void WorkspaceManager::onWorkspaceFocusIn()
{
	WorkspaceController *workspace = qobject_cast<WorkspaceController *>(sender());
	if (workspace)
		setCurrentWorkspace(workspace);
}

void WorkspaceManager::onWorkspaceShouldBeDeleted(WorkspaceController *workspace)
{
	removeWorkspace(workspace);
}

}
