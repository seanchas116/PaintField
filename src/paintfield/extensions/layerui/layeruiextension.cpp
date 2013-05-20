#include <QtPlugin>
#include <QMenu>

#include "paintfield/core/appcontroller.h"
#include "paintfield/core/settingsmanager.h"
#include "paintfield/core/widgets/simplebutton.h"
#include "layertreesidebar.h"
#include "layeruicontroller.h"

#include "layeruiextension.h"

namespace PaintField
{

const QString _layerTreeSidebarName = "paintfield.sidebar.layerTree";

LayerUIExtension::LayerUIExtension(Canvas *canvas, QObject *parent) :
    CanvasExtension(canvas, parent)
{
	LayerUIController *uiController = canvas ? new LayerUIController(canvas->document(), canvas) : 0;
	
	if (uiController)
	{
		for (auto action : uiController->actions())
			addAction(action);
	}
	
	auto sideBar = new LayerTreeSidebar(uiController, 0);
	addSideBar(_layerTreeSidebarName, sideBar);
}

void LayerUIExtensionFactory::initialize(AppController *app)
{
	app->settingsManager()->declareMenu("paintfield.layer", tr("Layer"));
	
	app->settingsManager()->declareAction("paintfield.layer.newRaster", tr("New Layer"));
	app->settingsManager()->declareAction("paintfield.layer.newGroup", tr("New Group"));
	app->settingsManager()->declareAction("paintfield.layer.import", tr("Import..."));
	app->settingsManager()->declareAction("paintfield.layer.merge", tr("Merge"));
	app->settingsManager()->declareAction("paintfield.layer.rasterize", tr("Rasterize"));
	
	app->settingsManager()->declareSideBar(_layerTreeSidebarName, SideBarInfo(tr("Layers")));
}

}
