#ifndef EXTENSIONMODULEFACTORY_H
#define EXTENSIONMODULEFACTORY_H

#include "paintfield-core/module.h"

namespace PaintField
{

class BuiltInExtensionFactory : public ExtensionFactory
{
	Q_OBJECT
	
public:
	
	BuiltInExtensionFactory(QObject *parent = 0);
	void initialize(AppController *app) { Q_UNUSED(app) }
};

}

#endif // EXTENSIONMODULEFACTORY_H
