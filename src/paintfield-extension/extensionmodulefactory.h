#ifndef EXTENSIONMODULEFACTORY_H
#define EXTENSIONMODULEFACTORY_H

#include "paintfield-core/module.h"

namespace PaintField
{

class ExtensionModuleFactory : public ModuleFactory
{
	Q_OBJECT
	
public:
	
	ExtensionModuleFactory(QObject *parent = 0);
	void initialize(AppController *app) { Q_UNUSED(app) }
};

}

#endif // EXTENSIONMODULEFACTORY_H
