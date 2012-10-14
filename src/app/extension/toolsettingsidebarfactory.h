#ifndef TOOLSETTINGSIDEBARFACTORY_H
#define TOOLSETTINGSIDEBARFACTORY_H

#include "core/sidebarfactory.h"

namespace PaintField
{

class ToolSettingSidebarFactory : public SidebarFactory
{
	Q_OBJECT
public:
	explicit ToolSettingSidebarFactory(QObject *parent = 0);
	
	QWidget *createSidebar(WorkspaceController *workspace, QWidget *parent);
	
signals:
	
public slots:
	
};

}

#endif // TOOLSETTINGSIDEBARFACTORY_H
