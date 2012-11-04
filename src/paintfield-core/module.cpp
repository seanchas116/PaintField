#include "module.h"

namespace PaintField
{

Tool *Module::createTool(const QString &name, CanvasView *view)
{
	Q_UNUSED(name)
	Q_UNUSED(view)
	return 0;
}

QWidget *Module::createSideBar(const QString &name)
{
	Q_UNUSED(name)
	return 0;
}

void Module::updateToolBar(QToolBar *toolBar, const QString &name)
{
	Q_UNUSED(toolBar)
	Q_UNUSED(name)
}

AppModuleList ModuleFactory::createAppModules(Application *app, QObject *parent)
{
	Q_UNUSED(app)
	Q_UNUSED(parent)
	return AppModuleList();
}

WorkspaceModuleList ModuleFactory::createWorkspaceModules(WorkspaceController *workspace, QObject *parent)
{
	Q_UNUSED(workspace)
	Q_UNUSED(parent)
	return WorkspaceModuleList();
}

CanvasModuleList ModuleFactory::createCanvasModules(CanvasController *canvas, QObject *parent)
{
	Q_UNUSED(canvas)
	Q_UNUSED(parent)
	return CanvasModuleList();
}

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

QWidget *createSideBarForWorkspace(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const QString &name)
{
	QWidget *sidebar;
	
	for (Module *module : appModules)
	{
		sidebar = module->createSideBar(name);
		if (sidebar)
			return sidebar;
	}
	for (Module *module : workspaceModules)
	{
		sidebar = module->createSideBar(name);
		if (sidebar)
			return sidebar;
	}
	return 0;
}

QWidget *createSideBarForCanvas(const CanvasModuleList &canvasModules, const QString &name)
{
	QWidget *sidebar;
	
	for (Module *module : canvasModules)
	{
		sidebar = module->createSideBar(name);
		if (sidebar)
			return sidebar;
	}
	return 0;
}

void updateToolBar(const AppModuleList &appModules, const WorkspaceModuleList &workspaceModules, const CanvasModuleList &canvasModules, QToolBar *toolBar, const QString &name)
{
	for (Module *module : appModules)
		module->updateToolBar(toolBar, name);
	
	for (Module *module : workspaceModules)
		module->updateToolBar(toolBar, name);
	
	for (Module *module : canvasModules)
		module->updateToolBar(toolBar, name);
}

}
