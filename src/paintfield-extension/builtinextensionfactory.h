#pragma once
#include "paintfield-core/extension.h"

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
