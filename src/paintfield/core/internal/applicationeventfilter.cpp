#include <QtGui>
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
			
			bool accepted = sendTabletEventToWindow(window, tabletEvent);
			
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
bool ApplicationEventFilter::sendTabletEventToWindow(QWidget *window, QTabletEvent *event)
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
	
	if (_targetWidget)
	{
		sendWidgetTabletEvent(&newEvent, _targetWidget);
		
		if (event->type() == QEvent::TabletRelease)
			_targetWidget = 0;
		
		return newEvent.isAccepted();
	}
	
	QWidget *widget = window->childAt(newEvent.posInt);
	setPrevWidget(widget);
	
	if (!widget)
		widget = window;
	
	if (event->type() == QEvent::TabletPress)
		_lastPressedWidget = widget;
	
	if (event->type() == QEvent::TabletRelease)
		_lastPressedWidget = 0;
	
	if (_lastPressedWidget && _lastPressedWidget != widget)
		newEvent.globalData.pressure = 0;
	
	forever
	{
		if (!widget)
			break;
		
		sendWidgetTabletEvent(&newEvent, widget);
		
		if (newEvent.isAccepted())
		{
			if (event->type() == QEvent::TabletPress)
			{
				_targetWidget = widget;
			}
			break;
		}
		widget = widget->parentWidget();
	}
	
	return newEvent.isAccepted();
}

void ApplicationEventFilter::sendWidgetTabletEvent(WidgetTabletEvent *event, QWidget *widget)
{
	event->posInt = widget->mapFromGlobal(event->globalPosInt);
	//QApplication::sendEvent(widget, event);
	static_cast<QObject *>(widget)->event(event);
}

void ApplicationEventFilter::setPrevWidget(QWidget *widget)
{
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
}

#endif

}
