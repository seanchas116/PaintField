#pragma once
#include "paintfield/core/extension.h"

namespace PaintField {

class SelectionBrushSidebarViewModel;
class ObservableVariantMap;
class MVVMViewModel;

class SelectionExtension : public WorkspaceExtension
{
	Q_OBJECT
public:
	SelectionExtension(Workspace *workspace, QObject *parent);

	MVVMViewModel *brushSidebarViewModel() { return mBrushSidebarViewModel; }
	Tool *createTool(const QString &name, Canvas *canvas) override;

private:
	ObservableVariantMap *mBrushState = nullptr;
	MVVMViewModel *mBrushSidebarViewModel = nullptr;
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
