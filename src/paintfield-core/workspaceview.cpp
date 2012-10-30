#include <QtGui>

#include "workspaceview.h"

namespace PaintField
{

void WorkspaceMenuAction::setBackendAction(QAction *action)
{
	if (action == _backendAction)
		return;
	
	if (_backendAction)
	{
		disconnect(_backendAction, 0, this, 0);
		disconnect(this, 0, _backendAction, 0);
	}
	
	_backendAction = action;
	
	if (action)
	{
		connect(action, SIGNAL(changed()), this, SLOT(onBackendActionChanged()));
		
		if (action->isCheckable())
		{
			setCheckable(true);
			connect(action, SIGNAL(triggered(bool)), this, SLOT(setChecked(bool)));
			connect(this, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		}
		else
		{
			connect(this, SIGNAL(triggered()), action, SLOT(trigger()));
		}
		onBackendActionChanged();
	}
}

void WorkspaceMenuAction::onBackendActionChanged()
{
	Q_ASSERT(_backendAction);
	setEnabled(_backendAction->isEnabled());
}


WorkspaceView::WorkspaceView(QWidget *parent) :
    QMainWindow(parent)
{
}

void WorkspaceView::addSidebarFrame(const QString &id, const QString &text, Qt::DockWidgetArea area)
{
	for (QDockWidget *each : _dockWidgets)
	{
		if (each->objectName() == id)
			return;
	}
	
	QDockWidget *dockWidget = new QDockWidget(text);
	dockWidget->setObjectName(id);
	
	addDockWidget(area, dockWidget);
	
	_dockWidgets << dockWidget;
}

void WorkspaceView::setSidebar(const QString &id, QWidget *sidebar)
{
	for (QDockWidget *dockWidget : _dockWidgets)
	{
		if (dockWidget->objectName() == id)
		{
			if (dockWidget->widget())
				dockWidget->widget()->deleteLater();
			
			dockWidget->setWidget(sidebar);
			return;
		}
	}
	sidebar->deleteLater();
}

void WorkspaceView::addToolBar(const QString &id, const QString &text, Qt::ToolBarArea area)
{
	auto toolBar = new QToolBar(text);
	toolBar->setObjectName(id);
	QMainWindow::addToolBar(area, toolBar);
	_toolBars << toolBar;
}

QToolBar *WorkspaceView::toolBar(const QString &id)
{
	for (auto toolBar : _toolBars)
	{
		if (toolBar->objectName() == id)
			return toolBar;
	}
	return 0;
}


}
