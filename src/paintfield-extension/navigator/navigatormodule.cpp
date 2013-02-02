#include "paintfield-core/appcontroller.h"
#include "paintfield-core/canvascontroller.h"
#include "paintfield-core/settingsmanager.h"
#include "navigatorcontroller.h"

#include "navigatormodule.h"

namespace PaintField
{

const QString _navigatorSidebarName = "paintfield.sidebar.navigator";

NavigatorExtension::NavigatorExtension(Canvas *canvas, QObject *parent) :
    CanvasExtension(canvas, parent)
{
	auto controller = new NavigatorController(canvas, this);
	addSideBar(_navigatorSidebarName, controller->view());
}

void NavigatorExtensionFactory::initialize(AppController *app)
{
	app->settingsManager()->declareSideBar(_navigatorSidebarName, SideBarInfo(tr("Navigator")));
}

CanvasExtensionList NavigatorExtensionFactory::createCanvasExtensions(Canvas *canvas, QObject *parent)
{
	return { new NavigatorExtension(canvas, parent) };
}

}
