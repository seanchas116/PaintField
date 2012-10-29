#ifndef TOOLUIMODULE_H
#define TOOLUIMODULE_H

#include "paintfield-core/module.h"

namespace PaintField
{

class ToolUIModule : public WorkspaceModule
{
	Q_OBJECT
public:
	explicit ToolUIModule(WorkspaceController *parent);
	
	QToolBar *createToolbar(const QString &name) override;
};

class ToolUIModuleFactory : public ModuleFactory
{
public:
	void initialize(Application *app) override;
	QList<WorkspaceModule *> createWorkspaceModules(WorkspaceController *workspace) override
	{
		return { new ToolUIModule(workspace) };
	}
};

}

#endif // TOOLUIMODULE_H
