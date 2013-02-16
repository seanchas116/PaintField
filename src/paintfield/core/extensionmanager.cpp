#include <QMetaObject>

#include "appcontroller.h"
#include "extension.h"

#include "extensionmanager.h"

namespace PaintField
{

ExtensionManager::ExtensionManager(QObject *parent) :
	QObject(parent)
{
}

void ExtensionManager::addExtensionFactory(ExtensionFactory *factory)
{
	_factories << factory;
	
	for (auto subFactory : factory->subExtensionFactories())
		addExtensionFactory(subFactory);
}

void ExtensionManager::initialize(AppController *app)
{
	for (auto factory : _factories)
		factory->initialize(app);
}

QList<AppExtension *> ExtensionManager::createAppExtensions(AppController *app, QObject *parent)
{
	QList<AppExtension *> modules;
	
	for (ExtensionFactory *factory : _factories)
		modules += factory->createAppExtensions(app, parent);
	
	return modules;
}

QList<WorkspaceExtension  *> ExtensionManager::createWorkspaceExtensions(Workspace *workspace, QObject *parent)
{
	QList<WorkspaceExtension *> modules;
	
	for (ExtensionFactory *factory : _factories)
		modules += factory->createWorkspaceExtensions(workspace, parent);
	
	
	return modules;
}

QList<CanvasExtension *> ExtensionManager::createCanvasExtensions(Canvas *canvas, QObject *parent)
{
	QList<CanvasExtension *> modules;
	
	for (ExtensionFactory *factory : _factories)
		modules += factory->createCanvasExtensions(canvas, parent);
	
	return modules;
}

}
