#include <QToolBar>
#include "paintfield-core/workspacecontroller.h"
#include "paintfield-core/appcontroller.h"
#include "paintfield-core/toolmanager.h"
#include "tooluimodule.h"

namespace PaintField
{

static const QString _toolToolbarName = "paintfield.toolbar.tool";

ToolUIModule::ToolUIModule(Workspace *workspace, QObject *parent) :
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

void ToolUIModuleFactory::initialize(AppController *app)
{
	app->settingsManager()->declareToolbar(_toolToolbarName, ToolbarDeclaration(QObject::tr("Tools")));
	//app->declareSidebar(_toolSettingSidebarName, SidebarInfo(tr("Tool Settings")));
}

}
