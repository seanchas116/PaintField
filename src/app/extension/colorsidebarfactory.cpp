#include "core/workspacecontroller.h"
#include "core/palettemanager.h"

#include "colorsidebar.h"

#include "colorsidebarfactory.h"

namespace PaintField
{

ColorSidebarFactory::ColorSidebarFactory(QObject *parent) :
	SidebarFactory(parent)
{
	setObjectName("paintfield.sidebar.color");
	setText(tr("Color"));
}

QWidget *ColorSidebarFactory::createSidebar(WorkspaceController *workspace, QWidget *parent)
{
	ColorSidebar *sidebar = new ColorSidebar(parent);
	
	PaletteManager *paletteManager = workspace->paletteManager();
	
	connect(paletteManager, SIGNAL(colorChanged(int,Malachite::Color)), sidebar, SLOT(setColor(int,Malachite::Color)));
	connect(paletteManager, SIGNAL(currentIndexChanged(int)), sidebar, SLOT(setCurrentIndex(int)));
	connect(sidebar, SIGNAL(currentIndexChanged(int)), paletteManager, SLOT(setCurrentIndex(int)));
	
	sidebar->setCurrentIndex(paletteManager->currentIndex());
	
	for (int i = 0; i < paletteManager->colorCount(); ++i)
	{
		sidebar->setColor(i, paletteManager->color(i));
	}
	
	return sidebar;
}

}
