#include "core/application.h"
#include "actionmodule.h"

#include "generalactioncontroller.h"

namespace PaintField
{

GeneralActionController::GeneralActionController(QObject *parent) :
    QObject(parent)
{
	ActionManager *actionManager = ActionModule::actionManager();
	
	actionManager->addAction(new QAction(this), "minimizeWindow", this, SLOT(minimizeCurrentWindow()));
	actionManager->addAction(new QAction(this), "zoomWindow", this, SLOT(zoomCurrentWindow()));
}

void GeneralActionController::minimizeCurrentWindow()
{
	QWidget *window = app()->activeWindow();
	if (window)
		window->showMinimized();
}

void GeneralActionController::zoomCurrentWindow()
{
	QWidget *window = app()->activeWindow();
	if (window)
		window->showMaximized();
}

}
