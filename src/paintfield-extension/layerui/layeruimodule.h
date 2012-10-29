#ifndef LAYERUIMODULE_H
#define LAYERUIMODULE_H

#include "paintfield-core/module.h"

namespace PaintField
{

class LayerActionController;

class LayerUIModule : public CanvasModule
{
	Q_OBJECT
public:
	explicit LayerUIModule(CanvasController *parent);
	
	QWidget *createSidebar(const QString &name) override;
	
private:
	
	LayerActionController *_actionController;
};

class LayerUIModuleFactory : public ModuleFactory
{
public:
	
	void initialize(Application *app) override;
	
	QList<CanvasModule *> createCanvasModules(CanvasController *canvas) override
	{
		return { new LayerUIModule(canvas) };
	}
};

}

#endif // LAYERUIMODULE_H
