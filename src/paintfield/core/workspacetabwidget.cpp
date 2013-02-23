#include "workspace.h"
#include "workspaceview.h"
#include "appcontroller.h"
#include "workspacemanager.h"

#include "workspacetabwidget.h"

namespace PaintField {

struct WorkspaceTabWidget::Data
{
	Workspace *workspace = 0;
};

WorkspaceTabWidget::WorkspaceTabWidget(Workspace *workspace, QWidget *parent) :
    DockTabWidget(workspace->view(), parent),
    d(new Data)
{
	d->workspace = workspace;
	
	connect(appController()->workspaceManager(), SIGNAL(currentWorkspaceChanged(Workspace*)),
	        this, SLOT(onCurrentWorkspaceChanged(Workspace*)));
}

WorkspaceTabWidget::~WorkspaceTabWidget()
{
	delete d;
}

Workspace *WorkspaceTabWidget::workspace()
{
	return d->workspace;
}

void WorkspaceTabWidget::onCurrentWorkspaceChanged(Workspace *workspace)
{
	if (isFloating())
		setVisible(workspace == d->workspace);
}


} // namespace PaintField
