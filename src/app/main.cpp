#include <QtGui>

#include "core/application.h"

int main(int argc, char *argv[])
{
	PaintField::Application application(argc, argv);
	
	application.loadMenuBarOrderFromJson(":/menubar.json");
	application.loadPanelOrderFromJson(":/panels.json");
	application.loadKeyMapFromJson(":/keymap.json");
	
	application.workspaceManager()->newWorkspace();
	
	return application.exec();
}
