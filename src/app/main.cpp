#include <QtGui>

#include "paintfield-core/application.h"
#include "paintfield-extension/extensionmodulefactory.h"

using namespace PaintField;

int main(int argc, char *argv[])
{
	Application a(argc, argv);
	
	a.loadMenuBarOrderFromJson(":/menubar.json");
	a.loadWorkspaceItemOrderFromJson(":/panels.json");
	a.loadKeyMapFromJson(":/keymap.json");
	
	a.addModuleFactory(new ExtensionModuleFactory);
	
	return a.exec();
}
