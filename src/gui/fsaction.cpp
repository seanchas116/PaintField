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

FSCanvasAction::FSCanvasAction(QObject *parent) :
	FSAction(parent)
{
	connect(fsGuiMain(), SIGNAL(currentCanvasChanged(FSCanvas*)), this, SLOT(onCanvasChanged(FSCanvas*)));
	onCanvasChanged(fsGuiMain()->currentCanvas());
}

void FSCanvasAction::onCanvasChanged(FSCanvas *canvas)
{
	setEnabled(canvas);
}

void FSLayerAction::onCanvasChanged(FSCanvas *canvas)
{
	if (_canvas)
	{
		disconnect(document(), SIGNAL(currentIndexChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentLayerChanged(QModelIndex)));
		disconnect(document()->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onLayerSelectionChanged(QItemSelection)));
	}
	_canvas = canvas;
	if (_canvas)
	{
		connect(_canvas, SIGNAL(destroyed()), this, SLOT(onViewDeleted()));
		
		connect(document(), SIGNAL(currentIndexChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentLayerChanged(QModelIndex)));
		onCurrentLayerChanged(document()->currentIndex());
		connect(document()->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onLayerSelectionChanged(QItemSelection)));
		onLayerSelectionChanged(document()->selectionModel()->selection());
	}
	else
	{
		onCurrentLayerChanged(QModelIndex());
		onLayerSelectionChanged(QItemSelection());
	}
}

void FSLayerAction::onViewDeleted()
{
	_canvas = 0;
}

void FSLayerAction::onCurrentLayerChanged(const QModelIndex &index)
{
	setEnabled(_canvas && index.isValid());
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

