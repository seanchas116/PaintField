#include <QtGui>

#include "core/application.h"
#include "extension/extensionmodule.h"

using namespace PaintField;

int main(int argc, char *argv[])
{
	Application application(argc, argv);
	
	application.loadMenuBarOrderFromJson(":/menubar.json");
	application.loadPanelOrderFromJson(":/panels.json");
	application.loadKeyMapFromJson(":/keymap.json");
	
	ExtensionModule extension;
	extension.initialize();
	
	application.workspaceManager()->newWorkspace();
	
	return application.exec();
}
