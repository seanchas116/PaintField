#include "module.h"

namespace PaintField
{

Tool *createTool(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const CanvasModuleList &canvasModules, const QString &name, CanvasView *view)
{
	Tool *tool;
	for (Module *module : appModules)
	{
		tool = module->createTool(name, view);
		if (tool)
			return tool;
	}
	for (Module *module : workspaceModules)
	{
		tool = module->createTool(name, view);
		if (tool)
			return tool;
	}
	for (Module *module : canvasModules)
	{
		tool = module->createTool(name, view);
		if (tool)
			return tool;
	}
	return 0;
}

QWidget *createSidebarForWorkspace(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const QString &name)
{
	QWidget *sidebar;
	
	for (Module *module : appModules)
	{
		sidebar = module->createSidebar(name);
		if (sidebar)
			return sidebar;
	}
	for (Module *module : workspaceModules)
	{
		sidebar = module->createSidebar(name);
		if (sidebar)
			return sidebar;
	}
	return 0;
}

QWidget *createSidebarForCanvas(const CanvasModuleList &canvasModules, const QString &name)
{
	QWidget *sidebar;
	
	for (Module *module : canvasModules)
	{
		sidebar = module->createSidebar(name);
		if (sidebar)
			return sidebar;
	}
	return 0;
}

}
