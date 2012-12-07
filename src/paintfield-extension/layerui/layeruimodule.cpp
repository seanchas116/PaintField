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
    CanvasModule(canvas, parent),
    _treeSidebar(new LayerTreeSidebar(canvas))
{
	if (canvas)
	{
		_actionController = new LayerActionController(canvas);
		
		addAction(_actionController->importAction());
		addAction(_actionController->newRasterAction());
		addAction(_actionController->newGroupAction());
		addAction(_actionController->mergeAction());
		
		auto sidebar = _treeSidebar;
		
		QMenu *addMenu = new QMenu(sidebar);
		
		addMenu->addAction(_actionController->newRasterAction());
		addMenu->addAction(_actionController->newGroupAction());
		addMenu->addAction(_actionController->importAction());
		
		sidebar->addButton()->setMenu(addMenu);
		
		connect(sidebar->removeButton(), SIGNAL(pressed()), _actionController, SLOT(removeLayers()));
		
		QMenu *miscMenu = new QMenu(sidebar);
		miscMenu->addAction(_actionController->mergeAction());
		
		sidebar->miscButton()->setMenu(miscMenu);
	}
}

QWidget *LayerUIModule::sideBar(const QString &name)
{
	if (name == _layerTreeSidebarName)
		return _treeSidebar;
	return 0;
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
