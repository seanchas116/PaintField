#include "layertreesidebar.h"
#include "layeractioncontroller.h"
#include "core/widgets/simplebutton.h"

#include "layertreesidebarfactory.h"

namespace PaintField
{

LayerTreeSidebarFactory::LayerTreeSidebarFactory(QObject *parent) :
	SidebarFactory(parent)
{
	setObjectName("paintfield.sidebar.layerTree");
	setText(tr("Layers"));
}

QWidget *LayerTreeSidebarFactory::createSidebarForCanvas(CanvasController *canvas, QWidget *parent)
{
	LayerTreeSidebar *sidebar = new LayerTreeSidebar(canvas->document()->layerModel(), parent);
	
	LayerActionController *actionController = canvas->findChild<LayerActionController *>();
	Q_ASSERT(actionController);
	
	QMenu *addMenu = new QMenu(sidebar);
	
	addMenu->addAction(actionController->newRasterAction());
	addMenu->addAction(actionController->newGroupAction());
	addMenu->addAction(actionController->importAction());
	
	sidebar->addButton()->setMenu(addMenu);
	
	connect(sidebar->removeButton(), SIGNAL(pressed()), actionController, SLOT(removeLayers()));
	
	QMenu *miscMenu = new QMenu(sidebar);
	miscMenu->addAction(actionController->mergeAction());
	
	sidebar->miscButton()->setMenu(miscMenu);
	
	return sidebar;
}

}
