#ifndef LAYERUIMODULE_H
#define LAYERUIMODULE_H

#include "paintfield-core/smartpointer.h"
#include "paintfield-core/module.h"

namespace PaintField
{

class LayerUIController;
class LayerTreeSidebar;

class LayerUIExtension : public CanvasExtension
{
	Q_OBJECT
public:
	LayerUIExtension(Canvas *canvas, QObject *parent);
	
};

class LayerUIExtensionFactory : public ExtensionFactory
{
	Q_OBJECT
public:
	
	LayerUIExtensionFactory(QObject *parent = 0) : ExtensionFactory(parent) {}
	
	void initialize(AppController *app) override;
	
	CanvasExtensionList createCanvasExtensions(Canvas *canvas, QObject *parent) override
	{
		return { new LayerUIExtension(canvas, parent) };
	}

private:
	
	QString _name;
};

}

#endif // LAYERUIMODULE_H
