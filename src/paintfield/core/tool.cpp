#include "tool.h"

namespace PaintField
{

struct Tool::Data
{
	LayerConstList layerDelegations;
	
	QList<LayerInsertion> layerInsertions;
	
	QCursor cursor;
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

void Tool::addLayerInsertion(const Layer *parent, int index, const Layer *layer)
{
	LayerInsertion insertion = { .parent = parent, .index = index, .layer = layer };
	d->layerInsertions << insertion;
}

void Tool::clearLayerInsertions()
{
	for (auto insertion : d->layerInsertions)
		delete insertion.layer;
	
	d->layerInsertions.clear();
}

QList<Tool::LayerInsertion> Tool::layerInsertions() const
{
	return d->layerInsertions;
}

void Tool::addLayerDelegation(const Layer *layer)
{
	d->layerDelegations << layer;
}

void Tool::clearLayerDelegation()
{
	d->layerDelegations.clear();
}

LayerConstList Tool::layerDelegations() const
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

}

