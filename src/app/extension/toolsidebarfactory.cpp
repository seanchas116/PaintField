#include "core/workspacecontroller.h"

#include "toolsidebar.h"

#include "toolsidebarfactory.h"

namespace PaintField
{

ToolSidebarFactory::ToolSidebarFactory(QObject *parent) :
	SidebarFactory(parent)
{
	setObjectName("paintfield.sidebar.tool");
	setText(tr("Tools"));
}

QWidget *ToolSidebarFactory::createSidebar(WorkspaceController *workspace, QWidget *parent)
{
	return new ToolSidebar(workspace->toolManager(), parent);
}

}
