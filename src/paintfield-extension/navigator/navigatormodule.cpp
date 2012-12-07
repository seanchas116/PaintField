#include "paintfield-core/appcontroller.h"
#include "paintfield-core/canvascontroller.h"
#include "navigatorcontroller.h"

#include "navigatormodule.h"

namespace PaintField
{

const QString _navigatorSidebarName = "paintfield.sidebar.navigator";

NavigatorModule::NavigatorModule(CanvasController *canvas, QObject *parent) :
    CanvasModule(canvas, parent),
    _controller(new NavigatorController(canvas, this))
{
}

QWidget *NavigatorModule::sideBar(const QString &name)
{
	if (name == _navigatorSidebarName)
		return _controller->view();
	return 0;
}

void NavigatorModuleFactory::initialize(AppController *app)
{
	app->declareSideBar(_navigatorSidebarName, SidebarDeclaration(tr("Navigator")));
}

CanvasModuleList NavigatorModuleFactory::createCanvasModules(CanvasController *canvas, QObject *parent)
{
	return { new NavigatorModule(canvas, parent) };
}

}
