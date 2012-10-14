#ifndef TOOLSIDEBARFACTORY_H
#define TOOLSIDEBARFACTORY_H

#include "core/sidebarfactory.h"

namespace PaintField
{

class ToolSidebarFactory : public SidebarFactory
{
	Q_OBJECT
public:
	explicit ToolSidebarFactory(QObject *parent = 0);
	
	QWidget *createSidebar(WorkspaceController *workspace, QWidget *parent);
	
signals:
	
public slots:
	
};

}

#endif // TOOLSIDEBARFACTORY_H
