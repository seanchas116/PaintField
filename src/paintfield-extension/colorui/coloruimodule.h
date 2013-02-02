#ifndef COLORMODULE_H
#define COLORMODULE_H

#include "paintfield-core/smartpointer.h"
#include "paintfield-core/module.h"

namespace PaintField
{

class ColorSidebar;

class ColorUIExtension : public WorkspaceExtension
{
	Q_OBJECT
public:
	ColorUIExtension(Workspace *workspace, QObject *parent);
	
signals:
	
public slots:
};

class ColorUIExtensionFactory : public ExtensionFactory
{
	Q_OBJECT
public:
	
	ColorUIExtensionFactory(QObject *parent = 0) : ExtensionFactory(parent) {}
	
	void initialize(AppController *app) override;
	
	QList<WorkspaceExtension *> createWorkspaceExtensions(Workspace *workspace, QObject *parent) override
	{
		return { new ColorUIExtension(workspace, parent) };
	}
};

}

#endif // COLORMODULE_H
