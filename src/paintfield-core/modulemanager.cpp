#include <QMetaObject>

#include "application.h"
#include "module.h"

#include "modulemanager.h"

namespace PaintField
{

ModuleManager::ModuleManager(QObject *parent) :
	QObject(parent)
{
}

void ModuleManager::addModuleFactory(ModuleFactory *factory)
{
	_factories << factory;
	
	for (auto subFactory : factory->subModuleFactories())
		addModuleFactory(subFactory);
}

void ModuleManager::initialize(Application *app)
{
	for (auto factory : _factories)
		factory->initialize(app);
}

QList<AppModule *> ModuleManager::createAppModules(Application *app, QObject *parent)
{
	QList<AppModule *> modules;
	
	for (ModuleFactory *factory : _factories)
		modules += factory->createAppModules(app, parent);
	
	return modules;
}

QList<WorkspaceModule  *> ModuleManager::createWorkspaceModules(WorkspaceController *workspace, QObject *parent)
{
	QList<WorkspaceModule *> modules;
	
	for (ModuleFactory *factory : _factories)
		modules += factory->createWorkspaceModules(workspace, parent);
	
	
	return modules;
}

QList<CanvasModule *> ModuleManager::createCanvasModules(CanvasController *canvas, QObject *parent)
{
	QList<CanvasModule *> modules;
	
	for (ModuleFactory *factory : _factories)
		modules += factory->createCanvasModules(canvas, parent);
	
	return modules;
}

}
