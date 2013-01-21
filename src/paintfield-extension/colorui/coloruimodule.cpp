#include "paintfield-core/appcontroller.h"
#include "paintfield-core/workspacecontroller.h"
#include "paintfield-core/palettemanager.h"

#include "colorsidebar.h"

#include "coloruimodule.h"

namespace PaintField
{

const QString _colorSidebarName = "paintfield.sidebar.color";

ColorUIModule::ColorUIModule(WorkspaceController *workspace, QObject *parent) :
    WorkspaceModule(workspace, parent)
{
	auto sideBar = new ColorSidebar;
	addSideBar(_colorSidebarName, sideBar);
	
	PaletteManager *paletteManager = workspace->paletteManager();
	
	connect(paletteManager, SIGNAL(colorChanged(int,Malachite::Color)), sideBar, SLOT(setColor(int,Malachite::Color)));
	connect(paletteManager, SIGNAL(currentIndexChanged(int)), sideBar, SLOT(setCurrentIndex(int)));
	connect(sideBar, SIGNAL(colorChanged(int,Malachite::Color)), paletteManager, SLOT(setColor(int,Malachite::Color)));
	connect(sideBar, SIGNAL(currentIndexChanged(int)), paletteManager, SLOT(setCurrentIndex(int)));
	
	sideBar->setCurrentIndex(paletteManager->currentIndex());
	
	for (int i = 0; i < paletteManager->colorCount(); ++i)
		sideBar->setColor(i, paletteManager->color(i));
}

void ColorUIModuleFactory::initialize(AppController *app)
{
	app->settingsManager()->declareSideBar(_colorSidebarName, SidebarDeclaration(QObject::tr("Color")));
}

}
