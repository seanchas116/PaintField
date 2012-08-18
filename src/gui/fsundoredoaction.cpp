#include "fsundoredoaction.h"

void FSUndoAction::onViewChanged(FSCanvasView *view)
{
	_undoAction.reset(view ? view->documentModel()->undoStack()->createUndoAction(0) : 0);
	associateAction(_undoAction.data());
}

void FSRedoAction::onViewChanged(FSCanvasView *view)
{
	_redoAction.reset(view ? view->documentModel()->undoStack()->createRedoAction(0) : 0);
	associateAction(_redoAction.data());
}
