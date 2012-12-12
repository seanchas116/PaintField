#ifndef LAYERUIMODULE_H
#define LAYERUIMODULE_H

#include "paintfield-core/smartpointer.h"
#include "paintfield-core/module.h"

namespace PaintField
{

class LayerUIController;
class LayerTreeSidebar;

class LayerUIModule : public CanvasModule
{
	Q_OBJECT
public:
	LayerUIModule(CanvasController *canvas, QObject *parent);
	
};

class LayerUIModuleFactory : public ModuleFactory
{
	Q_OBJECT
public:
	
	LayerUIModuleFactory(QObject *parent = 0) : ModuleFactory(parent) {}
	
	void initialize(AppController *app) override;
	
	CanvasModuleList createCanvasModules(CanvasController *canvas, QObject *parent) override
	{
		return { new LayerUIModule(canvas, parent) };
	}

private:
	
	QString _name;
};

}

#endif // LAYERUIMODULE_H
