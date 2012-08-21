#include <QtGui>
#include "fsguimain.h"

#include "fsaction.h"

void FSAction::associateAction(QAction *action)
{
	if (_associatedAction)
	{
		disconnect(_associatedAction, 0, this, 0);
		disconnect(this, 0, _associatedAction, 0);
	}
	
	_associatedAction = action;
	
	if (action)
	{
		connect(action, SIGNAL(changed()), this, SLOT(onAssociatedActionChanged()));
		connect(action, SIGNAL(destroyed()), this, SLOT(onAssociatedActionDeleted()));
		
		connect(this, SIGNAL(toggled(bool)), action, SLOT(setChecked(bool)));
		connect(this, SIGNAL(triggered()), action, SLOT(trigger()));
		
		setChecked(action->isChecked());
		onAssociatedActionChanged();
	}
}

void FSAction::onAssociatedActionChanged()
{
	setEnabled(_associatedAction->isEnabled());
	setText(_associatedAction->text());
}

void FSAction::onAssociatedActionDeleted()
{
	_associatedAction = 0;
}

FSCanvasViewAction::FSCanvasViewAction(QObject *parent) :
	FSAction(parent)
{
	connect(fsGuiMain(), SIGNAL(currentViewChanged(FSCanvasView*)), this, SLOT(onViewChanged(FSCanvasView*)));
	onViewChanged(fsGuiMain()->currentView());
}

void FSCanvasViewAction::onViewChanged(FSCanvasView *view)
{
	setEnabled(view);
}

void FSLayerAction::onViewChanged(FSCanvasView *view)
{
	if (_view)
	{
		disconnect(documentModel(), SIGNAL(currentIndexChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentLayerChanged(QModelIndex)));
		disconnect(documentModel()->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onLayerSelectionChanged(QItemSelection)));
	}
	_view = view;
	if (_view)
	{
		connect(_view, SIGNAL(destroyed()), this, SLOT(onViewDeleted()));
		
		connect(documentModel(), SIGNAL(currentIndexChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentLayerChanged(QModelIndex)));
		onCurrentLayerChanged(documentModel()->currentIndex());
		connect(documentModel()->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onLayerSelectionChanged(QItemSelection)));
		onLayerSelectionChanged(documentModel()->selectionModel()->selection());
	}
	else
	{
		onCurrentLayerChanged(QModelIndex());
		onLayerSelectionChanged(QItemSelection());
	}
}

void FSLayerAction::onViewDeleted()
{
	_view = 0;
}

void FSLayerAction::onCurrentLayerChanged(const QModelIndex &index)
{
	setEnabled(_view && index.isValid());
}

FSEditAction::FSEditAction(QObject *parent) :
	FSAction(parent)
{
	connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusWidgetChanged(QWidget*)));
}

void FSEditAction::onFocusWidgetChanged(QWidget *widget)
{
	if (!widget)
	{
		associateAction(0);
		setEnabled(false);
		return;
	}
	
	foreach (QAction *action, widget->actions())
	{
		if (action->objectName() == objectName())
		{
			associateAction(action);
			return;
		}
	}
	
	associateAction(0);
	setEnabled(false);
}

