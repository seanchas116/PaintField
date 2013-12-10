#pragma once
#include "paintfield/core/extension.h"

namespace PaintField {

class SelectionExtension : public WorkspaceExtension
{
	Q_OBJECT
public:
	SelectionExtension(Workspace *workspace, QObject *parent);
	Tool *createTool(const QString &name, Canvas *canvas) override;
};

class SelectionExtensionFactory : public ExtensionFactory
{
	Q_OBJECT
public:
	explicit SelectionExtensionFactory(QObject *parent = 0);
	void initialize(AppController *app) override;
	WorkspaceExtensionList createWorkspaceExtensions(Workspace *workspace, QObject *parent) override;
};

} // namespace PaintField
