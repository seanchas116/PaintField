#ifndef PAINTFIELD_VECTORTOOLSEXTENSION_H
#define PAINTFIELD_VECTORTOOLSEXTENSION_H

#include "paintfield/core/extension.h"

namespace PaintField {

class VectorToolsExtension : public WorkspaceExtension
{
	Q_OBJECT
public:
	explicit VectorToolsExtension(Workspace *workspace, QObject *parent = 0);
	
signals:
	
public slots:
	
};

class VectorToolsExtensionFactory : public ExtensionFactory
{
public:
	
	explicit VectorToolsExtensionFactory(QObject *parent = 0) : ExtensionFactory(parent) {}
	
	void initialize(AppController *app) override;
	
	WorkspaceExtensionList createWorkspaceExtensions(Workspace *workspace, QObject *parent) override
	{
		return { new VectorToolsExtension(workspace, parent) };
	}
};

} // namespace PaintField

#endif // PAINTFIELD_VECTORTOOLSEXTENSION_H
