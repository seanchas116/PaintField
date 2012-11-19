#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include "module.h"

namespace PaintField
{

class ModuleManager : public QObject
{
	Q_OBJECT
public:
	explicit ModuleManager(QObject *parent = 0);
	
	void addModuleFactory(ModuleFactory *factory);
	
	void initialize(AppController *app);
	
	QList<AppModule *> createAppModules(AppController *app, QObject *parent);
	QList<WorkspaceModule *> createWorkspaceModules(WorkspaceController *workspace, QObject *parent);
	QList<CanvasModule *> createCanvasModules(CanvasController *canvas, QObject *parent);
	
signals:
	
public slots:
	
private:
	
	QList<ModuleFactory *> _factories;
};

}

#endif // MODULEMANAGER_H
