#include "workspace.h"
#include "workspaceview.h"
#include "appcontroller.h"
#include "workspacemanager.h"

#include "workspacetabwidget.h"

namespace PaintField {

struct WorkspaceTabWidget::Data
{
	Workspace *workspace = 0;
	bool floating = false;
};

WorkspaceTabWidget::WorkspaceTabWidget(Workspace *workspace, QWidget *parent) :
    DockTabWidget(parent),
    d(new Data)
{
	d->workspace = workspace;
	
	connect(appController()->workspaceManager(), SIGNAL(currentWorkspaceChanged(Workspace*)),
	        this, SLOT(onCurrentWorkspaceChanged(Workspace*)));
	
	if (parent == 0)
		setFloating(true);
}

WorkspaceTabWidget::~WorkspaceTabWidget()
{
	delete d;
}

bool WorkspaceTabWidget::isFloating() const
{
	return d->floating;
}

void WorkspaceTabWidget::setFloating(bool floating)
{
	d->floating = floating;
	
	if (floating)
	{
		setParent(d->workspace->view());
		setWindowFlags(Qt::Tool);
	}
}

Workspace *WorkspaceTabWidget::workspace()
{
	return d->workspace;
}

void WorkspaceTabWidget::onCurrentWorkspaceChanged(Workspace *workspace)
{
	if (d->floating)
		setVisible(workspace == d->workspace);
}

bool WorkspaceTabWidget::event(QEvent *event)
{
	if (event->type() == QEvent::ParentChange)
	{
		if (parent() != d->workspace->view() && parent() != 0)
			setFloating(false);
	}
	
	return super::event(event);
}



} // namespace PaintField
