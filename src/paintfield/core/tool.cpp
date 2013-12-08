#include <QGraphicsItem>

#include "layerscene.h"
#include "document.h"

#include "tool.h"

namespace PaintField
{

struct Tool::Data
{
	QList<LayerConstRef> mLayerDelegations;
	
	QList<LayerInsertion> mLayerInsertions;
	
	QCursor mCursor;
	QScopedPointer<QGraphicsItem> mGraphicsItem;
	
	bool mEditing = false;

	SelectionShowMode mSelectionShowMode = SelectionShowModeDotted;

	int mCurrentCursorId = CursorIdDefault;
};

Tool::Tool(Canvas *parent) :
	QObject(parent),
	d(new Data)
{}

Tool::~Tool()
{
	clearLayerInsertions();
}

LayerConstRef Tool::currentLayer()
{
	return canvas()->document()->layerScene()->current();
}

void Tool::clearLayerInsertions()
{
	d->mLayerInsertions.clear();
}

QList<Tool::LayerInsertion> Tool::layerInsertions() const
{
	return d->mLayerInsertions;
}

void Tool::addLayerDelegation(const LayerConstRef &layer)
{
	d->mLayerDelegations << layer;
}

void Tool::clearLayerDelegation()
{
	d->mLayerDelegations.clear();
}

QList<LayerConstRef> Tool::layerDelegations() const
{
	return d->mLayerDelegations;
}

QCursor Tool::cursor() const
{
	return d->mCursor;
}

void Tool::setCursor(const QCursor &cursor)
{
	d->mCursor = cursor;
}

QGraphicsItem *Tool::graphicsItem()
{
	return d->mGraphicsItem.data();
}

void Tool::setGraphicsItem(QGraphicsItem *item)
{
	d->mGraphicsItem.reset(item);
}

void Tool::setEditing(bool editing)
{
	if (d->mEditing != editing)
	{
		d->mEditing = editing;
		emit editingChanged(editing);
	}
}

bool Tool::isEditing() const
{
	return d->mEditing;
}

SelectionShowMode Tool::selectionShowMode() const
{
	return d->mSelectionShowMode;
}

void Tool::setSelectionShowMode(SelectionShowMode mode)
{
	d->mSelectionShowMode = mode;
}

void Tool::toolEvent(QEvent *event)
{
	switch((int)event->type())
	{
		case QEvent::KeyPress:
			keyPressEvent(static_cast<QKeyEvent *>(event));
			return;
		case QEvent::KeyRelease:
			keyReleaseEvent(static_cast<QKeyEvent *>(event));
			return;
		case EventCanvasCursorPress:
			d->mCurrentCursorId = cursorPressEvent(static_cast<CanvasCursorEvent *>(event));
			return;
		case EventCanvasCursorMove:
			cursorMoveEvent(static_cast<CanvasCursorEvent *>(event), d->mCurrentCursorId);
			return;
		case EventCanvasCursorRelease:
			cursorReleaseEvent(static_cast<CanvasCursorEvent *>(event), d->mCurrentCursorId);
			return;
		default:
			return;
	}
}

void Tool::addLayerInsertion(const LayerConstRef &parent, int index, const LayerRef &layer)
{
	LayerInsertion insertion;
	insertion.parent = parent;
	insertion.index = index;
	insertion.layer = layer;
	d->mLayerInsertions << insertion;
}

int Tool::currentCursorId() const
{
	return d->mCurrentCursorId;
}

}

