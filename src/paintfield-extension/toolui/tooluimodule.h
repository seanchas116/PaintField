#ifndef TOOLUIMODULE_H
#define TOOLUIMODULE_H

#include "paintfield-core/module.h"

namespace PaintField
{

class ToolUIModule : public WorkspaceModule
{
	Q_OBJECT
public:
	explicit ToolUIModule(Workspace *workspace, QObject *parent);
	
	void updateToolBar(QToolBar *toolBar, const QString &name) override;
};

class ToolUIModuleFactory : public ModuleFactory
{
	Q_OBJECT
public:
	
	ToolUIModuleFactory(QObject *parent = 0) : ModuleFactory(parent) {}
	
	void initialize(AppController *app) override;
	QList<WorkspaceModule *> createWorkspaceModules(Workspace *workspace, QObject *parent) override
	{
		return { new ToolUIModule(workspace, parent) };
	}
};

}

#endif // TOOLUIMODULE_H
