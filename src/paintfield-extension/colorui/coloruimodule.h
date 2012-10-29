#ifndef COLORMODULE_H
#define COLORMODULE_H

#include "paintfield-core/module.h"

namespace PaintField
{

class ColorUIModule : public WorkspaceModule
{
	Q_OBJECT
public:
	explicit ColorUIModule(WorkspaceController *parent = 0);
	
	QWidget *createSidebar(const QString &name) override;
	
signals:
	
public slots:
	
private:
};

class ColorUIModuleFactory : public ModuleFactory
{
public:
	
	void initialize(Application *app) override;
	
	QList<WorkspaceModule *> createWorkspaceModules(WorkspaceController *workspace) override
	{
		return { new ColorUIModule(workspace) };
	}
};

}

#endif // COLORMODULE_H
