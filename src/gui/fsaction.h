#ifndef FSACTION_H
#define FSACTION_H

#include <QAction>
#include "fscanvasview.h"

class FSAction : public QAction
{
	Q_OBJECT
	
public:
	
	FSAction(QObject *parent) : QAction(parent), _associatedAction(0) {}
	
protected:
	
	void associateAction(QAction *action);
	
private slots:
	
	void onAssociatedActionChanged();
	
private:
	
	QAction *_associatedAction;
};

/**
  The FSCanvasViewAction class provides representation of actions which performs on a canvas view / document.
  In the default implementation, a FSCanvasViewAction will disable itself when no view is opened.
  Reimplement notifyViewChanged() if you want to customize this operation.
*/
class FSCanvasViewAction : public FSAction
{
	Q_OBJECT
	
public:
	
	FSCanvasViewAction(QObject *parent);
	
protected slots:
	
	/**
	  This slot is called when the current view is changed.
	  When all view is closed, it is called with view = 0.
	*/
	virtual void onViewChanged(FSCanvasView *view);
};

/**
  The FSCanvasViewAction class provides representation of actions which performs on layers.
  In the default implementation, a FSLayerAction will disable itself when no layer is selected.
  Reimplement notifyCurrentLayerChanged() or notifyLayerSelectionChanged() if you want to customize this operation.
*/
class FSLayerAction : public FSCanvasViewAction
{
	Q_OBJECT
	
public:
	
	FSLayerAction(QObject *parent) :
		FSCanvasViewAction(parent), _view(0) {}
	
protected:
	
	/**
	  @return The current view's document model
	*/
	FSDocumentModel *documentModel() { return _view ? _view->documentModel() : 0; }
	
protected slots:
	
	void onViewChanged(FSCanvasView *view);
	
	/**
	  This slot is called when the current layer is changed.
	  When no document is open, this slot is called with index = QModelIndex() (an invalid index).
	*/
	virtual void onCurrentLayerChanged(const QModelIndex &index);
	
	/**
	  This slot is called when the layer selection is changed.
	  When no document is open, this slot is called with index = QItemSelection() (an empty selection).
	*/
	virtual void onLayerSelectionChanged(const QItemSelection &selection) { Q_UNUSED(selection); }
	
private:
	
	FSCanvasView *_view;
};

/**
  The FSEditAction class provides representation of edit actions like copy or paste, which effect is depended on the focus widget.
  When the focus widget is changed, The FSEditAction object searches the widget for the action which has the same name.
  Then it associates itself with that action found in the widget.
  If no action that has the same name is found, it will be disabled automatically.
*/
class FSEditAction : public FSAction
{
	Q_OBJECT
	
public:
	
	FSEditAction(QObject *parent);
	
protected slots:
	
	virtual void onFocusWidgetChanged(QWidget *widget);
	
private:
};

#endif // FSACTION_H
