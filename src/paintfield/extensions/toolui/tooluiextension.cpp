#include <QToolBar>
#include <QtPlugin>
#include <QAction>
#include "paintfield/core/workspace.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/toolmanager.h"
#include "paintfield/core/settingsmanager.h"
#include "tooluiextension.h"

namespace PaintField
{

static const QString _toolToolbarName = "paintfield.toolbar.tool";

ToolUIExtension::ToolUIExtension(Workspace *workspace, QObject *parent) :
	WorkspaceExtension(workspace, parent)
{
}

void ToolUIExtension::updateToolBar(QToolBar *toolBar, const QString &name)
{
	if (name == _toolToolbarName)
	{
		toolBar->clear();
		
		auto toolOrder = appController()->settingsManager()->value({"tool-order"}).toList();
		auto actions = workspace()->toolManager()->actions();
		
		for (auto toolNameVariant : toolOrder)
		{
			if (toolNameVariant.toString().isEmpty())
			{
				toolBar->addSeparator();
				continue;
			}
			
			QAction *addedAction = 0;
			
			for (auto action : actions)
			{
				if (action->objectName() == toolNameVariant.toString())
				{
					toolBar->addAction(action);
					addedAction = action;
					break;
				}
			}
			
			if (addedAction)
				actions.removeAll(addedAction);
		}
		
		if (actions.size())
			toolBar->addActions(actions);
	}
}

void ToolUIExtensionFactory::initialize(AppController *app)
{
	app->settingsManager()->declareToolbar(_toolToolbarName, ToolBarInfo(QObject::tr("Tools")));
	//app->declareSidebar(_toolSettingSidebarName, SidebarInfo(tr("Tool Settings")));
}

}

Q_EXPORT_PLUGIN2(paintfield-toolui, PaintField::ToolUIExtensionFactory)

