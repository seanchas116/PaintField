#include "fsundoredoaction.h"

void FSUndoAction::onViewChanged(FSCanvasView *view)
{
	_undoAction.reset(view->documentModel()->undoStack()->createUndoAction(0));
	associateAction(_undoAction.data());
}

void FSRedoAction::onViewChanged(FSCanvasView *view)
{
	_redoAction.reset(view->documentModel()->undoStack()->createRedoAction(0));
	associateAction(_redoAction.data());
}
