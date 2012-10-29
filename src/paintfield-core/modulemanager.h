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
	
	void initialize(Application *app);
	
	QList<AppModule *> createAppModules(Application *app);
	QList<WorkspaceModule *> createWorkspaceModules(WorkspaceController *workspace);
	QList<CanvasModule *> createCanvasModules(CanvasController *canvas);
	
signals:
	
public slots:
	
private:
	
	QList<ModuleFactory *> _factories;
};

}

#endif // MODULEMANAGER_H
