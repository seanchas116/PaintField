#include "actionmanager.h"
#include "generalactioncontroller.h"

#include "actionmodule.h"

namespace PaintField
{

ActionModule::ActionModule(QObject *parent) :
    ApplicationModuleBase(parent)
{
	setTitle("PaintField.Action");
}

void ActionModule::firstInitialize()
{
	new ActionManager(app());
	new GeneralActionController(app());
}

}
