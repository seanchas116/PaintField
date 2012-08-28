#ifndef FSUNDOREDOACTION_H
#define FSUNDOREDOACTION_H

#include "fsaction.h"

class FSUndoAction : public FSCanvasAction
{
public:
	FSUndoAction(QObject *parent) : FSCanvasAction(parent) {}
	
protected:
	
	void onCanvasChanged(FSCanvas *canvas);
	
private:
	QScopedPointer<QAction> _undoAction;
};

class FSRedoAction : public FSCanvasAction
{
public:
	FSRedoAction(QObject *parent) : FSCanvasAction(parent) {}
	
protected:
	
	void onCanvasChanged(FSCanvas *canvas);
	
private:
	QScopedPointer<QAction> _redoAction;
};

#endif // FSUNDOREDOACTION_H
