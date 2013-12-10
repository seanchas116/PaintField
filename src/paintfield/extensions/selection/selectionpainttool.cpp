#include "selectionpainttool.h"

#include "selectionstroker.h"
#include "paintfield/core/selection.h"

namespace PaintField {

struct SelectionPaintTool::Data
{
	QScopedPointer<SelectionStroker> mStroker;
};

SelectionPaintTool::SelectionPaintTool(Canvas *parent) :
	Tool(parent),
	d(new Data)
{
}

SelectionPaintTool::~SelectionPaintTool()
{
}

int SelectionPaintTool::cursorPressEvent(CanvasCursorEvent *event)
{
	d->mStroker.reset(new SelectionStroker(document()->selection()));
	d->mStroker->moveTo(event->data);
	return 0;
}

void SelectionPaintTool::cursorMoveEvent(CanvasCursorEvent *event, int id)
{
	Q_UNUSED(id);
	if (!d->mStroker) return;
	d->mStroker->lineTo(event->data);
}

void SelectionPaintTool::cursorReleaseEvent(CanvasCursorEvent *event, int id)
{
	Q_UNUSED(id); Q_UNUSED(event);
	if (!d->mStroker) return;
	d->mStroker->end();
	document()->selection()->commitSurface();
}


} // namespace PaintField
