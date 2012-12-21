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
			
			QWidget *window = _trackedWindow ? _trackedWindow : qobject_cast<QWidget *>(watched);
			
			if (!window)
				return true;
			
			bool accepted = sendTabletEvent(window, tabletEvent);
			
			event->setAccepted(accepted);
			return accepted;
		}
#endif
		case QEvent::TabletEnterProximity:
		case QEvent::TabletLeaveProximity:
		{
			QTabletEvent *tabletEvent = static_cast<QTabletEvent *>(event);
			
			if (tabletEvent->type() == QEvent::TabletEnterProximity)
			{
				_isTabletActive = true;
				emit tabletActiveChanged(true);
				emit tabletActivated();
				
				if (_tabletPointerType != tabletEvent->pointerType())
				{
					_tabletPointerType = tabletEvent->pointerType();
					emit tabletPointerTypeChanged(_tabletPointerType);
				}
				
				if (_tabletId != tabletEvent->uniqueId())
				{
					_tabletId = tabletEvent->uniqueId();
					emit tabletIdChanged(_tabletId);
				}
			}
			else
			{
				_isTabletActive = false;
				emit tabletActiveChanged(false);
				emit tabletDeactivated();
			}
			
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
	
	// start tracking widget
	if (window->hasMouseTracking() && event->type() == QEvent::TabletPress)
	{
		_trackedWindow = window;
		_tabletEventAcceptedInTargetWindow = true;
	}
	
	if (event->type() == QEvent::TabletRelease)
		_trackedWindow = 0;
	
	// does not handle tablet event if once the tracked widget ignored it
	if (_trackedWindow && !_tabletEventAcceptedInTargetWindow)
		return false;
	
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
	
	forever
	{
		if (!widget)
			return false;
		
		newEvent.posInt = widget->mapFromGlobal(newEvent.globalPosInt);
		QCoreApplication::sendEvent(widget, &newEvent);
		
		if (newEvent.isAccepted())
			break;
		
		widget = widget->parentWidget();
	}
	
	if (_trackedWindow && event->type() == QEvent::TabletPress)
		_tabletEventAcceptedInTargetWindow = newEvent.isAccepted();
	
	return newEvent.isAccepted();
}
#endif

}
