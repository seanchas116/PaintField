#include "workspacetabwidget.h"

namespace PaintField
{

WorkspaceTabWidget::WorkspaceTabWidget(WorkspaceView *workspaceView, Type type, QWidget *parent) :
	DockTabWidget(parent),
	_workspaceView(workspaceView),
	_type(type)
{
	workspaceView->registerTabWidget(this);
}

WorkspaceTabWidget::WorkspaceTabWidget(WorkspaceTabWidget *other, QWidget *parent) :
	DockTabWidget(other, parent),
	_workspaceView(other->_workspaceView),
	_type(other->_type)
{
	_workspaceView->registerTabWidget(this);
}

bool WorkspaceTabWidget::isInsertableFrom(DockTabWidget *other)
{
	auto cast = qobject_cast<WorkspaceTabWidget *>(other);
	return cast && cast->_workspaceView == _workspaceView && cast->_type == _type;
}

DockTabWidget *WorkspaceTabWidget::createAnother(QWidget *parent)
{
	return new WorkspaceTabWidget(this, parent);
}

}

