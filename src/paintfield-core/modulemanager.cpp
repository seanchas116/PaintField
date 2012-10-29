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

QList<AppModule *> ModuleManager::createAppModules(Application *app)
{
	QList<AppModule *> modules;
	
	for (ModuleFactory *factory : _factories)
		modules += factory->createAppModules(app);
	
	return modules;
}

QList<WorkspaceModule  *> ModuleManager::createWorkspaceModules(WorkspaceController *workspace)
{
	QList<WorkspaceModule *> modules;
	
	for (ModuleFactory *factory : _factories)
		modules += factory->createWorkspaceModules(workspace);
	
	
	return modules;
}

QList<CanvasModule *> ModuleManager::createCanvasModules(CanvasController *canvas)
{
	QList<CanvasModule *> modules;
	
	for (ModuleFactory *factory : _factories)
		modules += factory->createCanvasModules(canvas);
	
	return modules;
}

}
