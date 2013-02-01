#ifndef LAYERMOVETOOLMODULE_H
#define LAYERMOVETOOLMODULE_H

#include "paintfield-core/module.h"

namespace PaintField
{

class LayerMoveToolModule : public AppModule
{
	Q_OBJECT
public:
	LayerMoveToolModule(AppController *app, QObject *parent) : AppModule(app, parent) {}
	
	Tool *createTool(const QString &name, Canvas *canvas) override;
	
signals:
	
public slots:
	
};

class LayerMoveToolModuleFactory : public ModuleFactory
{
	Q_OBJECT
	
public:
	
	LayerMoveToolModuleFactory(QObject *parent = 0) : ModuleFactory(parent) {}
	
	void initialize(AppController *app) override;
	
	AppModuleList createAppModules(AppController *app, QObject *parent) override
	{
		return { new LayerMoveToolModule(app, parent) };
	}
};

}

#endif // LAYERMOVETOOLMODULE_H
