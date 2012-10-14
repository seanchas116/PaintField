#include <QtGui>

#include "workspaceview.h"

namespace PaintField
{

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
		}
	}
}

}
