#include <QMenu>

#include "paintfield-core/appcontroller.h"
#include "paintfield-core/widgets/simplebutton.h"
#include "layertreesidebar.h"
#include "layeractioncontroller.h"

#include "layeruimodule.h"

namespace PaintField
{

const QString _layerTreeSidebarName = "paintfield.sidebar.layerTree";

LayerUIModule::LayerUIModule(CanvasController *canvas, QObject *parent) :
    CanvasModule(canvas, parent)
{
	auto sideBar = new LayerTreeSidebar(canvas, 0);
	addSideBar(_layerTreeSidebarName, sideBar);
	
	if (canvas)
	{
		auto actionController = new LayerActionController(canvas);
		
		addAction(actionController->importAction());
		addAction(actionController->newRasterAction());
		addAction(actionController->newGroupAction());
		addAction(actionController->mergeAction());
		
		QMenu *addMenu = new QMenu(sideBar);
		
		addMenu->addAction(actionController->newRasterAction());
		addMenu->addAction(actionController->newGroupAction());
		addMenu->addAction(actionController->importAction());
		
		sideBar->addButton()->setMenu(addMenu);
		
		connect(sideBar->removeButton(), SIGNAL(pressed()), actionController, SLOT(removeLayers()));
		
		QMenu *miscMenu = new QMenu(sideBar);
		miscMenu->addAction(actionController->mergeAction());
		
		sideBar->miscButton()->setMenu(miscMenu);
	}
}

void LayerUIModuleFactory::initialize(AppController *app)
{
	app->declareMenu("paintfield.layer", tr("Layer"));
	
	app->declareAction("paintfield.layer.newRaster", tr("New Layer"));
	app->declareAction("paintfield.layer.newGroup", tr("New Group"));
	app->declareAction("paintfield.layer.import", tr("Import..."));
	app->declareAction("paintfield.layer.merge", tr("Merge"));
	
	app->declareSideBar(_layerTreeSidebarName, SidebarDeclaration(tr("Layers")));
}

}
