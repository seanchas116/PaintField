#include "paintfield-core/appcontroller.h"
#include "paintfield-core/workspacecontroller.h"
#include "paintfield-core/palettemanager.h"

#include "colorsidebar.h"

#include "coloruimodule.h"

namespace PaintField
{

const QString _colorSidebarName = "paintfield.sidebar.color";

ColorUIModule::ColorUIModule(WorkspaceController *workspace, QObject *parent) :
    WorkspaceModule(workspace, parent),
    _sidebar(new ColorSidebar)
{
	PaletteManager *paletteManager = workspace->paletteManager();
	
	connect(paletteManager, SIGNAL(colorChanged(int,Malachite::Color)), _sidebar, SLOT(setColor(int,Malachite::Color)));
	connect(paletteManager, SIGNAL(currentIndexChanged(int)), _sidebar, SLOT(setCurrentIndex(int)));
	connect(_sidebar, SIGNAL(colorChanged(int,Malachite::Color)), paletteManager, SLOT(setColor(int,Malachite::Color)));
	connect(_sidebar, SIGNAL(currentIndexChanged(int)), paletteManager, SLOT(setCurrentIndex(int)));
	
	_sidebar->setCurrentIndex(paletteManager->currentIndex());
	
	for (int i = 0; i < paletteManager->colorCount(); ++i)
	{
		_sidebar->setColor(i, paletteManager->color(i));
	}
}

QWidget *ColorUIModule::sideBar(const QString &name)
{
	if (name == _colorSidebarName)
		return _sidebar;
	return 0;
}

void ColorUIModuleFactory::initialize(AppController *app)
{
	app->declareSideBar(_colorSidebarName, SidebarDeclaration(QObject::tr("Color")));
}

}
