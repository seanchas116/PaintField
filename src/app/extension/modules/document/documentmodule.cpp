#include "core/application.h"
#include "documentmanager.h"

#include "documentmodule.h"

namespace PaintField
{

DocumentModule::DocumentModule(QObject *parent) :
    ApplicationModuleBase(parent)
{
	setTitle("PaintField.Document");
}

void DocumentModule::initialize()
{
	new DocumentManager(app());
}


}
