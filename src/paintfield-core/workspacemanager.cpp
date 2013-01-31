#include <QDesktopWidget>
#include "application.h"
#include "appcontroller.h"
#include "modulemanager.h"
#include "util.h"
#include "workspaceview.h"

#include "workspacemanager.h"

namespace PaintField
{

WorkspaceManager::WorkspaceManager(QObject *parent) :
	QObject(parent)
{
	connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusWidgetChanged(QWidget*,QWidget*)));
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
	
	emit workspaceAdded(workspace);
	
	maximizeWindowSize(workspace->view());
	
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

void WorkspaceManager::onFocusWidgetChanged(QWidget *old, QWidget *now)
{
	Q_UNUSED(old)
	
	if (!now)
		return;
	
	QWidget *window = now->topLevelWidget();
	
	for (WorkspaceController *workspace : _workspaces)
	{
		if (workspace->view() == window)
			setCurrentWorkspace(workspace);
	}
}

}
