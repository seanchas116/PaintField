#ifndef FSUNDOREDOACTION_H
#define FSUNDOREDOACTION_H

#include "fsaction.h"

class FSUndoAction : public FSCanvasViewAction
{
public:
	FSUndoAction(QObject *parent) : FSCanvasViewAction(parent) {}
	
protected:
	
	void onViewChanged(FSCanvasView *view);
	
private:
	QScopedPointer<QAction> _undoAction;
};

class FSRedoAction : public FSCanvasViewAction
{
public:
	FSRedoAction(QObject *parent) : FSCanvasViewAction(parent) {}
	
protected:
	
	void onViewChanged(FSCanvasView *view);
	
private:
	QScopedPointer<QAction> _redoAction;
};

#endif // FSUNDOREDOACTION_H
