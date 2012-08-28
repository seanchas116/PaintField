#include "fsundoredoaction.h"

void FSUndoAction::onCanvasChanged(FSCanvas *canvas)
{
	_undoAction.reset(canvas ? canvas->document()->undoStack()->createUndoAction(0) : 0);
	associateAction(_undoAction.data());
}

void FSRedoAction::onCanvasChanged(FSCanvas *canvas)
{
	_redoAction.reset(canvas ? canvas->document()->undoStack()->createRedoAction(0) : 0);
	associateAction(_redoAction.data());
}
