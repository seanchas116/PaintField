#include "workspacecontroller.h"
#include "workspaceview.h"
#include "appcontroller.h"
#include "workspacemanager.h"

#include "workspacetabwidget.h"

namespace PaintField {

struct WorkspaceTabWidget::Data
{
	WorkspaceController *workspace = 0;
	bool floating = false;
};

WorkspaceTabWidget::WorkspaceTabWidget(WorkspaceController *workspace, QWidget *parent) :
    DockTabWidget(parent),
    d(new Data)
{
	d->workspace = workspace;
	
	connect(appController()->workspaceManager(), SIGNAL(currentWorkspaceChanged(WorkspaceController*)),
	        this, SLOT(onCurrentWorkspaceChanged(WorkspaceController*)));
	
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

WorkspaceController *WorkspaceTabWidget::workspace()
{
	return d->workspace;
}

void WorkspaceTabWidget::onCurrentWorkspaceChanged(WorkspaceController *workspace)
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
