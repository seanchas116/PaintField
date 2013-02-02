#ifndef PAINTFIELD_ABOUTDIALOGMODULE_H
#define PAINTFIELD_ABOUTDIALOGMODULE_H

#include "paintfield-core/module.h"

namespace PaintField {

class AboutDialogExtension : public WorkspaceExtension
{
	Q_OBJECT
public:
	explicit AboutDialogExtension(Workspace *workspace, QObject *parent = 0);
	
signals:
	
public slots:
	
private slots:
	
	void showAboutDialog();
};

class AboutDialogExtensionFactory : public ExtensionFactory
{
	Q_OBJECT
public:
	
	AboutDialogExtensionFactory(QObject *parent = 0) : ExtensionFactory(parent) {}
	
	void initialize(AppController *app) override;
	QList<WorkspaceExtension *> createWorkspaceExtensions(Workspace *workspace, QObject *parent) override
	{
		return { new AboutDialogExtension(workspace, parent) };
	}
};

} // namespace PaintField

#endif // PAINTFIELD_ABOUTDIALOGMODULE_H
