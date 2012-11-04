#include "paintfield-core/application.h"
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
}

QWidget *ColorUIModule::createSideBar(const QString &name)
{
	if (name == _colorSidebarName)
	{
		ColorSidebar *sidebar = new ColorSidebar;
		
		PaletteManager *paletteManager = workspace()->paletteManager();
		
		connect(paletteManager, SIGNAL(colorChanged(int,Malachite::Color)), sidebar, SLOT(setColor(int,Malachite::Color)));
		connect(paletteManager, SIGNAL(currentIndexChanged(int)), sidebar, SLOT(setCurrentIndex(int)));
		connect(sidebar, SIGNAL(colorChanged(int,Malachite::Color)), paletteManager, SLOT(setColor(int,Malachite::Color)));
		connect(sidebar, SIGNAL(currentIndexChanged(int)), paletteManager, SLOT(setCurrentIndex(int)));
		
		sidebar->setCurrentIndex(paletteManager->currentIndex());
		
		for (int i = 0; i < paletteManager->colorCount(); ++i)
		{
			sidebar->setColor(i, paletteManager->color(i));
		}
		
		return sidebar;
	}
	return 0;
}

void ColorUIModuleFactory::initialize(Application *app)
{
	app->declareSideBar(_colorSidebarName, SidebarDeclaration(QObject::tr("Color")));
}

}
