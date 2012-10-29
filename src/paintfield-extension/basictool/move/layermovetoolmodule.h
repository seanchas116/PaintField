#ifndef LAYERMOVETOOLMODULE_H
#define LAYERMOVETOOLMODULE_H

#include "paintfield-core/module.h"

namespace PaintField
{

class LayerMoveToolModule : public AppModule
{
	Q_OBJECT
public:
	explicit LayerMoveToolModule(Application *parent = 0) : AppModule(parent) {}
	
	Tool *createTool(const QString &name, CanvasView *view) override;
	
signals:
	
public slots:
	
};

class LayerMoveToolModuleFactory : public ModuleFactory
{
public:
	
	void initialize(Application *app) override;
	
	QList<AppModule *> createAppModules(Application *app) override
	{
		return { new LayerMoveToolModule(app) };
	}
};

}

#endif // LAYERMOVETOOLMODULE_H
