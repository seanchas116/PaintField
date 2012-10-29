#ifndef EXTENSIONMODULEFACTORY_H
#define EXTENSIONMODULEFACTORY_H

#include "paintfield-core/module.h"

namespace PaintField
{

class ExtensionModuleFactory : public ModuleFactory
{
public:
	ExtensionModuleFactory();
	void initialize(Application *app) { Q_UNUSED(app) }
};

}

#endif // EXTENSIONMODULEFACTORY_H
