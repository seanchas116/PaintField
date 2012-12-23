#include <QtGui>
#include "../debug.h"
#include "../tabletevent.h"

#include "applicationeventfilter.h"

namespace PaintField
{

using namespace Malachite;

ApplicationEventFilter::ApplicationEventFilter(QObject *parent) :
    QObject(parent)
{}

bool ApplicationEventFilter::eventFilter(QObject *watched, QEvent *event)
{
	switch (event->type())
	{
		default:
			return false;
#ifdef PAINTFIELD_ENABLE_TABLET_EVENT_FILTER
		case QEvent::TabletMove:
		case QEvent::TabletPress:
		case QEvent::TabletRelease:
		{
			auto tabletEvent = static_cast<QTabletEvent *>(event);
			
			QWidget *window = qobject_cast<QWidget *>(watched);
			
			if (!window)
				return true;
			
			bool accepted = sendTabletEvent(window, tabletEvent);
			
			event->setAccepted(accepted);
			return accepted;
		}
#endif
		case QEvent::TabletEnterProximity:
		{
			emit tabletEntered(static_cast<QTabletEvent *>(event));
			return true;
		}
		case QEvent::TabletLeaveProximity:
		{
			emit tabletLeft(static_cast<QTabletEvent *>(event));
			return true;
		}
		case QEvent::FileOpen:
		{
			auto fileOpenEvent = static_cast<QFileOpenEvent *>(event);
			emit fileOpenRequested(fileOpenEvent->file());
			return true;
		}
	}
}

#ifdef PAINTFIELD_ENABLE_TABLET_EVENT_FILTER
bool ApplicationEventFilter::sendTabletEvent(QWidget *window, QTabletEvent *event)
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
	
	auto sendWidgetTabletEvent = [](WidgetTabletEvent *ev, QWidget *w)
	{
		ev->posInt = w->mapFromGlobal(ev->globalPosInt);
		QApplication::sendEvent(w, ev);
	};
	
	if (_trackedWidget)
	{
		sendWidgetTabletEvent(&newEvent, _trackedWidget);
		
		if (event->type() == QEvent::TabletRelease)
			_trackedWidget = 0;
		
		return newEvent.isAccepted();
	}
	
	QWidget *widget = window->childAt(newEvent.posInt);
	
	if (_prevWidget != widget)
	{
		if (_prevWidget)
		{
			QEvent leave(QEvent::Leave);
			QApplication::sendEvent(_prevWidget, &leave);
		}
		
		if (widget)
		{
			QEvent enter(QEvent::Enter);
			QApplication::sendEvent(widget, &enter);
		}
		
		_prevWidget = widget;
	}
	
	forever
	{
		if (!widget)
			return false;
		
		sendWidgetTabletEvent(&newEvent, widget);
		
		if (newEvent.isAccepted())
		{
			if (widget->hasMouseTracking() && event->type() == QEvent::TabletPress)
				_trackedWidget = widget;
			
			break;
		}
		widget = widget->parentWidget();
	}
	
	return newEvent.isAccepted();
}
#endif

}
