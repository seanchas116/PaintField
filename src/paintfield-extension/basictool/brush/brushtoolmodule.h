#ifndef BRUSHTOOLMODULE_H
#define BRUSHTOOLMODULE_H

#include "paintfield-core/module.h"

#include "brushsetting.h"

namespace PaintField
{

class BrushToolModule : public WorkspaceModule
{
	Q_OBJECT
public:
	BrushToolModule(WorkspaceController *workspace, QObject *parent);
	
	Tool *createTool(const QString &name, CanvasView *parent) override;
	QWidget *createSideBar(const QString &name) override;
	
private:
	BrushSetting _setting;
};

class BrushToolModuleFactory : public ModuleFactory
{
public:
	
	void initialize(AppController *app) override;
	
	WorkspaceModuleList createWorkspaceModules(WorkspaceController *workspace, QObject *parent) override
	{
		return { new BrushToolModule(workspace, parent) };
	}
};

}

#endif // BRUSHTOOLMODULE_H
