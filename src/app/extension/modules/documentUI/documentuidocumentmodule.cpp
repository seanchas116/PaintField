#include "documentcontroller.h"

#include "documentuidocumentmodule.h"

namespace PaintField
{

DocumentUIDocumentModule::DocumentUIDocumentModule(QObject *parent) :
    DocumentModuleBase(parent)
{
}

void DocumentUIDocumentModule::firstInitialize(Document *document)
{
	new DocumentController(document, this);
}

}
