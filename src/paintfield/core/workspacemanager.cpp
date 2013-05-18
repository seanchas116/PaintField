#include <QDesktopWidget>
#include "application.h"
#include "appcontroller.h"
#include "extensionmanager.h"
#include "settingsmanager.h"
#include "util.h"
#include "workspaceview.h"
#include "cpplinq-paintfield.h"

#include "workspacemanager.h"

namespace PaintField
{

WorkspaceManager::WorkspaceManager(QObject *parent) :
	QObject(parent)
{
	connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusWidgetChanged(QWidget*,QWidget*)));
}

void WorkspaceManager::closeAllAndQuit()
{
	using namespace cpplinq;
	QVariantList states = from(_workspaces) >> select([](Workspace *w)->QVariant{return w->view()->saveState();}) >> to_any_container<QVariantList>();
	
	for (Workspace *controller : _workspaces)
	{
		if (!controller->tryClose())
			return;	// failed to close
	}
	
	// saves workspace states and quit
	
	appController()->settingsManager()->setValue({"last-workspace-states"}, states);
	appController()->quit();
}

void WorkspaceManager::loadLastWorkspaces()
{
	auto lastWorkspaceStates = appController()->settingsManager()->value({"last-workspace-states"}).toList();
	
	if (lastWorkspaceStates.isEmpty())
	{
		addWorkspace(new Workspace(this));
	}
	else
	{
		for (auto &state : lastWorkspaceStates)
			addWorkspace(new Workspace(this), state.toMap());
	}
}

void WorkspaceManager::newWorkspace()
{
	addWorkspace(new Workspace(this));
}

void WorkspaceManager::setCurrentWorkspace(Workspace *workspace)
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

void WorkspaceManager::removeWorkspace(Workspace *workspace)
{
	if (_workspaces.contains(workspace))
	{
		if (_currentWorkspace == workspace)
			_currentWorkspace = 0;
		
		_workspaces.removeAll(workspace);
		emit workspaceAboutToBeRemoved(workspace);
		workspace->deleteLater();
	}
}

void WorkspaceManager::closeWorkspace(Workspace *workspace)
{
	if (_workspaces.contains(workspace))
	{
		bool willQuit = (_workspaces.size() == 1);
		
		if (willQuit)
		{
			QVariantList states = {workspace->view()->saveState()};
			appController()->settingsManager()->setValue({"last-workspace-states"}, states);
		}
		
		removeWorkspace(workspace);
		
		if (willQuit)
			appController()->quit();
	}
}

void WorkspaceManager::addWorkspace(Workspace *workspace, const QVariantMap &state)
{
	auto stateToSet = state;
	if (stateToSet.isEmpty())
		stateToSet = appController()->settingsManager()->value({"workspace-state-default"}).toMap();
	
	_workspaces << workspace;
	
	connect(workspace, SIGNAL(focused()), this, SLOT(onWorkspaceFocusIn()));
	connect(workspace, SIGNAL(shouldBeDeleted(Workspace*)), this, SLOT(closeWorkspace(Workspace*)));
	
	emit workspaceAdded(workspace);
	
	auto view = new WorkspaceView(workspace, stateToSet);
	
	Util::maximizeWindowSize(view);
	
	view->show();
	
	setCurrentWorkspace(workspace);
}

void WorkspaceManager::onWorkspaceFocusIn()
{
	Workspace *workspace = qobject_cast<Workspace *>(sender());
	if (workspace)
		setCurrentWorkspace(workspace);
}

void WorkspaceManager::onWorkspaceShouldBeDeleted(Workspace *workspace)
{
	removeWorkspace(workspace);
}

void WorkspaceManager::onFocusWidgetChanged(QWidget *old, QWidget *now)
{
	Q_UNUSED(old)
	
	if (!now)
		return;
	
	QWidget *window = now->topLevelWidget();
	
	for (Workspace *workspace : _workspaces)
	{
		if (workspace->view() == window)
			setCurrentWorkspace(workspace);
	}
}

}
