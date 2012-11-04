#include <QToolBar>
#include "paintfield-core/toolmanager.h"
#include "tooluimodule.h"

namespace PaintField
{

const QString _toolToolbarName = "paintfield.toolbar.tool";
//const QString _toolSettingSidebarName = "paintfield.sidebar.toolSetting";

ToolUIModule::ToolUIModule(WorkspaceController *workspace, QObject *parent) :
	WorkspaceModule(workspace, parent)
{
}

void ToolUIModule::updateToolBar(QToolBar *toolBar, const QString &name)
{
	if (name == _toolToolbarName)
	{
		toolBar->clear();
		toolBar->addActions(workspace()->toolManager()->actions());
	}
}

void ToolUIModuleFactory::initialize(Application *app)
{
	app->declareToolbar(_toolToolbarName, ToolbarDeclaration(QObject::tr("Tools")));
	//app->declareSidebar(_toolSettingSidebarName, SidebarInfo(tr("Tool Settings")));
}

}
