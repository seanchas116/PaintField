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
	if (canvas)
	{
		_actionController = new LayerActionController(canvas);
		
		addAction(_actionController->importAction());
		addAction(_actionController->newRasterAction());
		addAction(_actionController->newGroupAction());
		addAction(_actionController->mergeAction());
	}
}

QWidget *LayerUIModule::createSideBar(const QString &name)
{
	if (name == _layerTreeSidebarName)
	{
		if (canvas())
		{
			auto sidebar = new LayerTreeSidebar(canvas());
			
			QMenu *addMenu = new QMenu(sidebar);
			
			addMenu->addAction(_actionController->newRasterAction());
			addMenu->addAction(_actionController->newGroupAction());
			addMenu->addAction(_actionController->importAction());
			
			sidebar->addButton()->setMenu(addMenu);
			
			connect(sidebar->removeButton(), SIGNAL(pressed()), _actionController, SLOT(removeLayers()));
			
			QMenu *miscMenu = new QMenu(sidebar);
			miscMenu->addAction(_actionController->mergeAction());
			
			sidebar->miscButton()->setMenu(miscMenu);
			
			return sidebar;
		}
		else
			return new LayerTreeSidebar(0);
	}
	return 0;
}

void LayerUIModuleFactory::initialize(AppController *app)
{
	app->declareMenu("paintfield.layer", QObject::tr("Layer"));
	
	app->declareAction("paintfield.layer.newRaster", QObject::tr("New Layer"));
	app->declareAction("paintfield.layer.newGroup", QObject::tr("New Group"));
	app->declareAction("paintfield.layer.import", QObject::tr("Import..."));
	app->declareAction("paintfield.layer.merge", QObject::tr("Merge"));
	
	app->declareSideBar(_layerTreeSidebarName, SidebarDeclaration(QObject::tr("Layers")));
}

}
