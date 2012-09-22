#include "toolmodule.h"
#include "toolpanel.h"
#include "modules/mainwindow/mainwindowmodule.h"

namespace PaintField
{

ToolModule::ToolModule(QObject *parent) :
    ApplicationModuleBase(parent)
{
	setTitle("PaintField.Tool");
}

void ToolModule::initialize()
{
	new ToolManager(app());
	
	MainWindowModule::mainWindow()->addPanel(new ToolPanel());
}

}
