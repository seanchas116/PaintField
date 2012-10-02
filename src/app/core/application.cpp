#include <QtGui>

#include "application.h"

namespace PaintField
{

Application::Application(int &argv, char **args) :
    TabletApplication(argv, args)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	
	_workspaceManager = new WorkspaceManager(this);
	
	_workspaceManager->newWorkspace();
}

void Application::addToolFactory(ToolFactory *factory)
{
	if (factory)
	{
		factory->setParent(this);
		_toolFactories << factory;
	}
}


}
