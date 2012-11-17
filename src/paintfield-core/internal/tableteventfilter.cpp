#include <QtGui>
#include "../debug.h"
#include "../tabletevent.h"

#include "tableteventfilter.h"

namespace PaintField
{

using namespace Malachite;

bool TabletEventFilter::eventFilter(QObject *watched, QEvent *event)
{
	switch (event->type())
	{
		default:
			return false;
		case QEvent::TabletMove:
		case QEvent::TabletPress:
		case QEvent::TabletRelease:
			break;
	}
	
	auto tabletEvent = static_cast<QTabletEvent *>(event);
	
	QWidget *window = _targetWindow ? _targetWindow : qobject_cast<QWidget *>(watched);
	
	if (!window || window->parent() != 0)
		return true;
	
	event->setAccepted(sendTabletEvent(window, tabletEvent));
	return true;
}

bool TabletEventFilter::sendTabletEvent(QWidget *window, QTabletEvent *event)
{
	Q_CHECK_PTR(window);
	Q_CHECK_PTR(event);
	
	TabletInputData data(event->hiResGlobalPos(),
						 event->pressure(),
						 event->rotation(),
						 event->tangentialPressure(),
						 Vec2D(event->xTilt(), event->yTilt()));
	
	auto toNewEventType = [](QEvent::Type type)
	{
		switch (type)
		{
			default:
			case QEvent::TabletMove:
				return PaintField::EventWidgetTabletMove;
			case QEvent::TabletPress:
				return PaintField::EventWidgetTabletPress;
			case QEvent::TabletRelease:
				return PaintField::EventWidgetTabletRelease;
		}
	};
	
	WidgetTabletEvent newEvent(toNewEventType(event->type()), event->globalPos(), event->pos(), data, event->modifiers());
	newEvent.setAccepted(false);
	
	QWidget *widget = window->childAt(newEvent.posInt);
	if (!widget)
		return false;
	
	newEvent.posInt = widget->mapFromGlobal(newEvent.globalPosInt);
	QCoreApplication::sendEvent(widget, &newEvent);
	
	if (event->type() == QEvent::TabletPress)
		_targetWindow = window;
	
	if (event->type() == QEvent::TabletRelease)
		_targetWindow = 0;
	
	return newEvent.isAccepted();
}

}
