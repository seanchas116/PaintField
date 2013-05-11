#include <QtPlugin>

#include "paintfield/core/appcontroller.h"
#include "paintfield/core/workspace.h"
#include "paintfield/core/palettemanager.h"
#include "paintfield/core/settingsmanager.h"
#include "paintfield/core/colorbuttongroup.h"
#include "paintfield/core/widgets/colorbutton.h"

#include "colorsidebar.h"

#include "coloruiextension.h"

namespace PaintField
{

static const QString colorSidebarName = "paintfield.sidebar.color";

ColorUIExtension::ColorUIExtension(Workspace *workspace, QObject *parent) :
    WorkspaceExtension(workspace, parent)
{
	auto sidebar = new ColorSideBar();
	addSideBar(colorSidebarName, sidebar);
	
	auto paletteM = workspace->paletteManager();
	connect(paletteM, SIGNAL(colorChanged(int,Malachite::Color)), sidebar, SLOT(setColorButtonColor(int,Malachite::Color)));
	connect(sidebar, SIGNAL(colorButtonColorChanged(int,Malachite::Color)), paletteM, SLOT(setColor(int,Malachite::Color)));
	connect(sidebar, SIGNAL(colorButtonClicked(int)), paletteM, SLOT(setCurrentIndex(int)));
	
	for (int i = 0; i < paletteM->colorCount(); ++i)
		sidebar->setColorButtonColor(i, paletteM->color(i));
	
	auto group = workspace->colorButtonGroup();
	
	for (auto b : sidebar->colorButtons())
		group->add(b);
	
	connect(sidebar, SIGNAL(currentColorChanged(Malachite::Color)), group, SLOT(setCurrentColor(Malachite::Color)));
	connect(group, SIGNAL(currentColorChanged(Malachite::Color)), sidebar, SLOT(setCurrentColor(Malachite::Color)));
	
	group->setCurrentButton(sidebar->colorButtons().at(0));
	//sidebar->colorButtons().at(0)->setChecked(true);
}

void ColorUIExtensionFactory::initialize(AppController *app)
{
	app->settingsManager()->declareSideBar(colorSidebarName, SideBarInfo(tr("Color")));
}

}
