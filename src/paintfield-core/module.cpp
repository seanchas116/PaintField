#include <QWidget>

#include "module.h"

namespace PaintField
{

Extension::~Extension()
{
	for (auto sideBar : _sideBars)
		sideBar->deleteLater();
}

void Extension::addSideBar(const QString &id, QWidget *sideBar)
{
	Q_CHECK_PTR(sideBar);
	
	if (_sideBars.contains(id) && _sideBars.value(id) == sideBar)
	{
		sideBar->deleteLater();
		return;
	}
	
	_sideBars.insert(id, sideBar);
}

Tool *Extension::createTool(const QString &, Canvas *)
{
	return 0;
}

void Extension::updateToolBar(QToolBar *, const QString &)
{
}

AppExtensionList ExtensionFactory::createAppExtensions(AppController *, QObject *)
{
	return AppExtensionList();
}

WorkspaceExtensionList ExtensionFactory::createWorkspaceExtensions(Workspace *, QObject *)
{
	return WorkspaceExtensionList();
}

CanvasExtensionList ExtensionFactory::createCanvasExtensions(Canvas *, QObject *)
{
	return CanvasExtensionList();
}

namespace ExtensionUtil
{

Tool *createTool(const AppExtensionList &appModules, const WorkspaceExtensionList &workspaceExtensions, const CanvasExtensionList &canvasModules, const QString &name, Canvas *canvas)
{
	Tool *tool;
	for (Extension *module : appModules)
	{
		tool = module->createTool(name, canvas);
		if (tool)
			return tool;
	}
	for (Extension *module : workspaceExtensions)
	{
		tool = module->createTool(name, canvas);
		if (tool)
			return tool;
	}
	for (Extension *module : canvasModules)
	{
		tool = module->createTool(name, canvas);
		if (tool)
			return tool;
	}
	return 0;
}

QWidget *sideBarForWorkspace(const AppExtensionList &appExtensions, const WorkspaceExtensionList &workspaceExtensions, const QString &name)
{
	QWidget *sidebar;
	
	for (Extension *module : appExtensions)
	{
		sidebar = module->sideBar(name);
		if (sidebar)
			return sidebar;
	}
	for (Extension *module : workspaceExtensions)
	{
		sidebar = module->sideBar(name);
		if (sidebar)
			return sidebar;
	}
	return 0;
}

QWidget *sideBarForCanvas(const CanvasExtensionList &canvasExtensions, const QString &name)
{
	QWidget *sidebar;
	
	for (Extension *module : canvasExtensions)
	{
		sidebar = module->sideBar(name);
		if (sidebar)
			return sidebar;
	}
	return 0;
}

void updateToolBar(const AppExtensionList &appExtensions, const WorkspaceExtensionList &workspaceExtensions, const CanvasExtensionList &canvasExtensions, QToolBar *toolBar, const QString &name)
{
	for (Extension *module : appExtensions)
		module->updateToolBar(toolBar, name);
	
	for (Extension *module : workspaceExtensions)
		module->updateToolBar(toolBar, name);
	
	for (Extension *module : canvasExtensions)
		module->updateToolBar(toolBar, name);
}

}

}
