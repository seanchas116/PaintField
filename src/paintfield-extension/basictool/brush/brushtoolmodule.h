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
	explicit BrushToolModule(WorkspaceController *parent = 0);
	
	Tool *createTool(const QString &name, CanvasView *parent) override;
	QWidget *createSidebar(const QString &name) override;
	
private:
	BrushSetting _setting;
};

class BrushToolModuleFactory : public ModuleFactory
{
public:
	
	void initialize(Application *app) override;
	
	QList<WorkspaceModule *> createWorkspaceModules(WorkspaceController *workspace) override
	{
		return { new BrushToolModule(workspace) };
	}
};

}

#endif // BRUSHTOOLMODULE_H
