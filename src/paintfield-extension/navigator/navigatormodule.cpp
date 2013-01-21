#include "paintfield-core/appcontroller.h"
#include "paintfield-core/canvascontroller.h"
#include "navigatorcontroller.h"

#include "navigatormodule.h"

namespace PaintField
{

const QString _navigatorSidebarName = "paintfield.sidebar.navigator";

NavigatorModule::NavigatorModule(CanvasController *canvas, QObject *parent) :
    CanvasModule(canvas, parent)
{
	auto controller = new NavigatorController(canvas, this);
	addSideBar(_navigatorSidebarName, controller->view());
}

void NavigatorModuleFactory::initialize(AppController *app)
{
	app->settingsManager()->declareSideBar(_navigatorSidebarName, SidebarDeclaration(tr("Navigator")));
}

CanvasModuleList NavigatorModuleFactory::createCanvasModules(CanvasController *canvas, QObject *parent)
{
	return { new NavigatorModule(canvas, parent) };
}

}
