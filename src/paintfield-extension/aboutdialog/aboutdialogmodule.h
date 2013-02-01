#ifndef PAINTFIELD_ABOUTDIALOGMODULE_H
#define PAINTFIELD_ABOUTDIALOGMODULE_H

#include "paintfield-core/module.h"

namespace PaintField {

class AboutDialogModule : public WorkspaceModule
{
	Q_OBJECT
public:
	explicit AboutDialogModule(Workspace *workspace, QObject *parent = 0);
	
signals:
	
public slots:
	
private slots:
	
	void showAboutDialog();
};

class AboutDialogModuleFactory : public ModuleFactory
{
	Q_OBJECT
public:
	
	AboutDialogModuleFactory(QObject *parent = 0) : ModuleFactory(parent) {}
	
	void initialize(AppController *app) override;
	QList<WorkspaceModule *> createWorkspaceModules(Workspace *workspace, QObject *parent) override
	{
		return { new AboutDialogModule(workspace, parent) };
	}
};

} // namespace PaintField

#endif // PAINTFIELD_ABOUTDIALOGMODULE_H
