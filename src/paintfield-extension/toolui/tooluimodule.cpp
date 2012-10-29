#include <QToolBar>
#include "paintfield-core/toolmanager.h"
#include "tooluimodule.h"

namespace PaintField
{

const QString _toolToolbarName = "paintfield.sidebar.tool";
//const QString _toolSettingSidebarName = "paintfield.sidebar.toolSetting";

ToolUIModule::ToolUIModule(WorkspaceController *parent) :
	WorkspaceModule(parent)
{
}

QToolBar *ToolUIModule::createToolbar(const QString &name)
{
	if (name == _toolToolbarName)
	{
		QToolBar *toolBar = new QToolBar;
		toolBar->addActions(workspace()->toolManager()->actions());
		return toolBar;
	}
	return 0;
}

void ToolUIModuleFactory::initialize(Application *app)
{
	app->declareToolbar(_toolToolbarName, ToolbarInfo(QObject::tr("Tools")));
	//app->declareSidebar(_toolSettingSidebarName, SidebarInfo(tr("Tool Settings")));
}

}
