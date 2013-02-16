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
	LayerUIController *layerUIController;
	if (canvas)
		layerUIController = new LayerUIController(canvas);
	else
		layerUIController = 0;
	
	auto sideBar = new LayerTreeSidebar(layerUIController, 0);
	addSideBar(_layerTreeSidebarName, sideBar);
	
	if (layerUIController)
	{
		addAction(layerUIController->importAction());
		addAction(layerUIController->newRasterAction());
		addAction(layerUIController->newGroupAction());
		addAction(layerUIController->mergeAction());
	}
}

void LayerUIExtensionFactory::initialize(AppController *app)
{
	app->settingsManager()->declareMenu("paintfield.layer", tr("Layer"));
	
	app->settingsManager()->declareAction("paintfield.layer.newRaster", tr("New Layer"));
	app->settingsManager()->declareAction("paintfield.layer.newGroup", tr("New Group"));
	app->settingsManager()->declareAction("paintfield.layer.import", tr("Import..."));
	app->settingsManager()->declareAction("paintfield.layer.merge", tr("Merge"));
	
	app->settingsManager()->declareSideBar(_layerTreeSidebarName, SideBarInfo(tr("Layers")));
}

}

Q_EXPORT_PLUGIN2(paintfield-layerui, PaintField::LayerUIExtensionFactory)
