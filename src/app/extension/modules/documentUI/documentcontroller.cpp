#include "../action/actionmodule.h"

#include "documentcontroller.h"

namespace PaintField
{

DocumentController::DocumentController(Document *document, QObject *parent) :
    QObject(parent),
    _document(document)
{
	ActionManager *actionManager = ActionModule::actionManager();
	
	actionManager->addAction(new QAction(this), "closeDocument", this, SLOT(closeDocument()));
	actionManager->addAction(new QAction(this), "saveDocument", this, SLOT(saveDocument()));
	actionManager->addAction(new QAction(this), "saveAsDocument", this, SLOT(saveAsDocument()));
	actionManager->addAction(document->undoStack()->createRedoAction(this), "redoDocument");
	actionManager->addAction(document->undoStack()->createUndoAction(this), "undoDocument");
}

}
