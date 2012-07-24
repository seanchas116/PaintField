#include <QtGui>
#include "fsguimain.h"

#include "fsaction.h"

FSCanvasViewAction::FSCanvasViewAction(QObject *parent) :
	QAction(parent)
{
	connect(fsGuiMain(), SIGNAL(currentViewChanged(FSCanvasView*)), this, SLOT(notifyViewChanged(FSCanvasView*)));
	notifyViewChanged(fsGuiMain()->currentView());
}

void FSCanvasViewAction::notifyViewChanged(FSCanvasView *view)
{
	setEnabled(view);
}

void FSLayerAction::notifyViewChanged(FSCanvasView *view)
{
	if (_view)
	{
		disconnect(documentModel(), SIGNAL(currentIndexChanged(QModelIndex,QModelIndex)), this, SLOT(notifyCurrentLayerChanged(QModelIndex)));
		disconnect(documentModel()->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(notifyLayerSelectionChanged(QItemSelection)));
	}
	_view = view;
	if (_view)
	{
		connect(documentModel(), SIGNAL(currentIndexChanged(QModelIndex,QModelIndex)), this, SLOT(notifyCurrentLayerChanged(QModelIndex)));
		notifyCurrentLayerChanged(documentModel()->currentIndex());
		connect(documentModel()->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(notifyLayerSelectionChanged(QItemSelection)));
		notifyLayerSelectionChanged(documentModel()->selectionModel()->selection());
	}
	else
	{
		notifyCurrentLayerChanged(QModelIndex());
		notifyLayerSelectionChanged(QItemSelection());
	}
}

void FSLayerAction::notifyCurrentLayerChanged(const QModelIndex &index)
{
	setEnabled(_view && index.isValid());
}

FSEditAction::FSEditAction(QObject *parent) :
	QAction(parent),
	_action(0)
{
	connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(notifyFocusWidgetChanged(QWidget*)));
}

void FSEditAction::notifyFocusWidgetChanged(QWidget *widget)
{
	disconnectAction();
	
	if (!widget)
	{
		connectAction(0);
		setEnabled(false);
		return;
	}
	
	foreach (QAction *action, widget->actions())
	{
		if (action->objectName() == objectName())
		{
			connectAction(action);
			return;
		}
	}
	
	connectAction(0);
	setEnabled(false);
}

void FSEditAction::actionChanged()
{
	setEnabled(_action->isEnabled());
	setText(_action->text());
}

void FSEditAction::connectAction(QAction *action)
{
	_action = action;
	
	if (action)
	{
		connect(action, SIGNAL(toggled(bool)), this, SLOT(setChecked(bool)));
		connect(action, SIGNAL(triggered()), this, SLOT(trigger()));
		connect(action, SIGNAL(changed()), this, SLOT(actionChanged()));
	}
}

void FSEditAction::disconnectAction()
{
	if (_action)
		disconnect(_action, 0, this, 0);
}

