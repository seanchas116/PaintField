#ifndef LAYERMOVETOOLMODULE_H
#define LAYERMOVETOOLMODULE_H

#include "paintfield-core/module.h"

namespace PaintField
{

class LayerMoveToolModule : public AppModule
{
	Q_OBJECT
public:
	LayerMoveToolModule(Application *app, QObject *parent) : AppModule(app, parent) {}
	
	Tool *createTool(const QString &name, CanvasView *view) override;
	
signals:
	
public slots:
	
};

class LayerMoveToolModuleFactory : public ModuleFactory
{
public:
	
	void initialize(Application *app) override;
	
	AppModuleList createAppModules(Application *app, QObject *parent) override
	{
		return { new LayerMoveToolModule(app, parent) };
	}
};

}

#endif // LAYERMOVETOOLMODULE_H
