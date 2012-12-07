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
	if (canvas)
	{
		_navigatorController = new NavigatorController(canvas->view(), canvas);
	}
}

QWidget *NavigatorModule::createSideBar(const QString &name)
{
	if (name == _navigatorSidebarName)
	{
		if (canvas())
		{
			return _navigatorController->createView();
		}
		else
		{
			auto view = new NavigatorView;
			view->setEnabled(false);
			return view;
		}
	}
	else
	{
		return 0;
	}
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
