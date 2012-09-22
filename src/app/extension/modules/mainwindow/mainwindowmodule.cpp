#include "core/application.h"

#include "mainwindow.h"

#include "mainwindowmodule.h"

namespace PaintField
{

MainWindowModule::MainWindowModule(QObject *parent) :
    ApplicationModuleBase(parent)
{
	setTitle("PaintField.MainWindow");
	
	addDependency(MetaInfo("PaintField.Document"));
	addDependency(MetaInfo("PaintField.Action"));
}

void MainWindowModule::initialize()
{
	_mainWindow = new MainWindow();
	connect(app(), SIGNAL(modulesLoaded()), this, SLOT(onAppModulesLoaded()));
}

void MainWindowModule::onAppModulesLoaded()
{
	_mainWindow->arrangeMenus();
	_mainWindow->arrangePanels();
	
	_mainWindow->show();
}

MainWindow *MainWindowModule::_mainWindow;

}
