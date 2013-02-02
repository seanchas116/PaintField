#pragma once

#include "paintfield-core/extension.h"

namespace PaintField
{

class ToolUIExtension : public WorkspaceExtension
{
	Q_OBJECT
public:
	explicit ToolUIExtension(Workspace *workspace, QObject *parent);
	
	void updateToolBar(QToolBar *toolBar, const QString &name) override;
};

class ToolUIExtensionFactory : public ExtensionFactory
{
	Q_OBJECT
public:
	
	ToolUIExtensionFactory(QObject *parent = 0) : ExtensionFactory(parent) {}
	
	void initialize(AppController *app) override;
	QList<WorkspaceExtension *> createWorkspaceExtensions(Workspace *workspace, QObject *parent) override
	{
		return { new ToolUIExtension(workspace, parent) };
	}
};

}

