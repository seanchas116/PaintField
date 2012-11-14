#include "tool.h"

namespace PaintField
{

void Tool::toolEvent(QEvent *event)
{
	switch(event->type())
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

