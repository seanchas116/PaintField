#include "core/workspacecontroller.h"
#include "toolsettingsidebar.h"

#include "toolsettingsidebarfactory.h"

namespace PaintField
{

ToolSettingSidebarFactory::ToolSettingSidebarFactory(QObject *parent) :
	SidebarFactory(parent)
{
	setObjectName("paintfield.sidebar.toolSetting");
	setText(tr("Tool Settings"));
}

QWidget *ToolSettingSidebarFactory::createSidebar(WorkspaceController *workspace, QWidget *parent)
{
	return new ToolSettingSidebar(workspace->toolManager(), parent);
}

}

