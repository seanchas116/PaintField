#include <QtGui>

#include "core/application.h"

#include "documentmanagercontroller.h"

#include "../document/documentmodule.h"
#include "documentuimodule.h"

namespace PaintField
{

DocumentUIModule::DocumentUIModule(QObject *parent) :
    DocumentApplicationModuleBase(parent)
{
	setTitle("PaintField.DocumentUI");
	
	addDependency(MetaInfo("PaintField.Document"));
	addDependency(MetaInfo("PaintField.Tool"));
}

void DocumentUIModule::initialize()
{
	// create controller
	
	new DocumentManagerController(this);
}

}
