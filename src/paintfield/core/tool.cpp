#include <QGraphicsItem>

#include "layerscene.h"
#include "document.h"

#include "tool.h"

namespace PaintField
{

struct Tool::Data
{
	QList<LayerConstRef> layerDelegations;
	
	QList<LayerInsertion> layerInsertions;
	
	QCursor cursor;
	QScopedPointer<QGraphicsItem> graphicsItem;
	
	bool editing = false;
};

Tool::Tool(Canvas *parent) :
	QObject(parent),
	d(new Data)
{}

Tool::~Tool()
{
	clearLayerInsertions();
	delete d;
}

LayerConstRef Tool::currentLayer()
{
	return canvas()->document()->layerScene()->current();
}

void Tool::clearLayerInsertions()
{
	d->layerInsertions.clear();
}

QList<Tool::LayerInsertion> Tool::layerInsertions() const
{
	return d->layerInsertions;
}

void Tool::addLayerDelegation(const LayerConstRef &layer)
{
	d->layerDelegations << layer;
}

void Tool::clearLayerDelegation()
{
	d->layerDelegations.clear();
}

QList<LayerConstRef> Tool::layerDelegations() const
{
	return d->layerDelegations;
}

QCursor Tool::cursor() const
{
	return d->cursor;
}

void Tool::setCursor(const QCursor &cursor)
{
	d->cursor = cursor;
}

QGraphicsItem *Tool::graphicsItem()
{
	return d->graphicsItem.data();
}

void Tool::setGraphicsItem(QGraphicsItem *item)
{
	d->graphicsItem.reset(item);
}

void Tool::setEditing(bool editing)
{
	if (d->editing != editing)
	{
		d->editing = editing;
		emit editingChanged(editing);
	}
}

bool Tool::isEditing() const
{
	return d->editing;
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
		case EventCanvasMouseMove:
			mouseMoveEvent(static_cast<CanvasMouseEvent *>(event));
			return;
		case EventCanvasMousePress:
			mousePressEvent(static_cast<CanvasMouseEvent *>(event));
			return;
		case EventCanvasMouseRelease:
			mouseReleaseEvent(static_cast<CanvasMouseEvent *>(event));
			return;
		case EventCanvasMouseDoubleClick:
			mouseDoubleClickEvent(static_cast<CanvasMouseEvent *>(event));
			return;
		case EventCanvasTabletMove:
			tabletMoveEvent(static_cast<CanvasTabletEvent *>(event));
			return;
		case EventCanvasTabletPress:
			tabletPressEvent(static_cast<CanvasTabletEvent *>(event));
			return;
		case EventCanvasTabletRelease:
			tabletReleaseEvent(static_cast<CanvasTabletEvent *>(event));
			return;
		default:
			return;
	}
}

void Tool::addLayerInsertion(const LayerConstRef &parent, int index, const LayerRef &layer)
{
	LayerInsertion insertion = { .parent = parent, .index = index, .layer = layer };
	d->layerInsertions << insertion;
}

}

