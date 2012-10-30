#ifndef TOOLUIMODULE_H
#define TOOLUIMODULE_H

#include "paintfield-core/module.h"

namespace PaintField
{

class ToolUIModule : public WorkspaceModule
{
	Q_OBJECT
public:
	explicit ToolUIModule(WorkspaceController *workspace, QObject *parent);
	
	void updateToolBar(QToolBar *toolBar, const QString &name) override;
};

class ToolUIModuleFactory : public ModuleFactory
{
public:
	void initialize(Application *app) override;
	QList<WorkspaceModule *> createWorkspaceModules(WorkspaceController *workspace, QObject *parent) override
	{
		return { new ToolUIModule(workspace, parent) };
	}
};

}

#endif // TOOLUIMODULE_H
