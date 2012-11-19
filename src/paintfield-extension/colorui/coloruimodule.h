#ifndef COLORMODULE_H
#define COLORMODULE_H

#include "paintfield-core/module.h"

namespace PaintField
{

class ColorUIModule : public WorkspaceModule
{
	Q_OBJECT
public:
	ColorUIModule(WorkspaceController *workspace, QObject *parent);
	
	QWidget *createSideBar(const QString &name) override;
	
signals:
	
public slots:
	
private:
};

class ColorUIModuleFactory : public ModuleFactory
{
public:
	
	void initialize(AppController *app) override;
	
	QList<WorkspaceModule *> createWorkspaceModules(WorkspaceController *workspace, QObject *parent) override
	{
		return { new ColorUIModule(workspace, parent) };
	}
};

}

#endif // COLORMODULE_H
