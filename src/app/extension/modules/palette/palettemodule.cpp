#include "colorpanel.h"
#include "palettemanager.h"

#include "core/application.h"
#include "modules/mainwindow/mainwindowmodule.h"

#include "palettemodule.h"

namespace PaintField
{

PaletteModule::PaletteModule(QObject *parent) :
    ApplicationModuleBase(parent)
{
	setTitle("PaintField.Palette");
}

void PaletteModule::initialize()
{
	new PaletteManager(app());
	MainWindowModule::mainWindow()->addPanel(new ColorPanel());
}

}
