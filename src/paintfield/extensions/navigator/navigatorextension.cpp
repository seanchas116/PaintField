#include <QtPlugin>

#include "paintfield/core/appcontroller.h"
#include "paintfield/core/canvas.h"
#include "paintfield/core/settingsmanager.h"
#include "navigatorview.h"

#include "navigatorextension.h"

namespace PaintField
{

static const QString _navigatorSidebarName = "paintfield.sidebar.navigator";

NavigatorExtension::NavigatorExtension(Canvas *canvas, QObject *parent) :
    CanvasExtension(canvas, parent)
{
	auto view = new NavigatorView(canvas);
	addSideBar(_navigatorSidebarName, view);
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
