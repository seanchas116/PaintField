#include <QtGui>
#include "fstabletevent.h"

#include "fstableteventfilter.h"

bool FSTabletEventFilter::eventFilter(QObject *watched, QEvent *event)
{
	Q_UNUSED(watched);
	
	if (event->type() == QEvent::TabletMove || event->type() == QEvent::TabletPress || event->type() == QEvent::TabletRelease)
	{
		QTabletEvent *tabletEvent = static_cast<QTabletEvent *>(event);
		
		QWidget *widget = _targetWidget ? _targetWidget : QApplication::widgetAt(tabletEvent->globalPos());
		
		if (!widget)
			return true;
		
		FSGlobal::Event newEventType;
		switch (event->type())
		{
		case QEvent::TabletMove:
			newEventType = FSGlobal::EventTabletMove;
			break;
		case QEvent::TabletPress:
			newEventType = FSGlobal::EventTabletPress;
			break;
		case QEvent::TabletRelease:
			newEventType = FSGlobal::EventTabletRelease;
			break;
		default:
			event->ignore();
			return true;
		}
		
		FSTabletEvent newEvent(newEventType,
							   tabletEvent->hiResGlobalPos(),
							   tabletEvent->globalPos(),
							   QPointF(),
							   tabletEvent->pressure(),
							   tabletEvent->xTilt(),
							   tabletEvent->yTilt(),
							   tabletEvent->rotation(),
							   tabletEvent->tangentialPressure(),
							   tabletEvent->modifiers());
		
		newEvent.setAccepted(false);
		
		QWidget *widgetRecursve = widget;
		
		while (widgetRecursve)
		{
			newEvent.data.pos = newEvent.globalPos - widgetRecursve->geometry().topLeft();
			
			QCoreApplication::sendEvent(widgetRecursve, &newEvent);
			
			if (newEvent.isAccepted())
				break;
			
			widgetRecursve = widgetRecursve->parentWidget();
		}
		
		event->setAccepted(newEvent.isAccepted());
		
		if (event->type() == QEvent::TabletPress)
		{
			_targetWidget = widget;
		}
		
		if (event->type() == QEvent::TabletRelease)
		{
			_targetWidget = 0;
		}
		
		return true;
	}
	
	return false;
}

/*
bool FSTabletEventFilter::eventFilter(QObject *watched, QEvent *event)
{
	Q_UNUSED(watched);
	
	if (event->type() == QEvent::TabletMove || event->type() == QEvent::TabletPress || event->type() == QEvent::TabletRelease) {
		
		QTabletEvent *tabletEvent = static_cast<QTabletEvent *>(event);
		
		QWidget *widget = QApplication::topLevelAt(tabletEvent->globalPos());
		if (!widget)
		{
			event->ignore();
			return true;
		}
		
		FSGlobal::Event newEventType;
		switch (event->type())
		{
		case QEvent::TabletMove:
			newEventType = FSGlobal::EventTabletMove;
			break;
		case QEvent::TabletPress:
			newEventType = FSGlobal::EventTabletPress;
			break;
		case QEvent::TabletRelease:
			newEventType = FSGlobal::EventTabletRelease;
			break;
		default:
			event->ignore();
			return true;
		}
		
		FSTabletEvent newEvent(newEventType,
							   tabletEvent->hiResGlobalPos(),
							   tabletEvent->globalPos(),
							   QPointF(),
							   tabletEvent->pressure(),
							   tabletEvent->xTilt(),
							   tabletEvent->yTilt(),
							   tabletEvent->rotation(),
							   tabletEvent->tangentialPressure(),
							   tabletEvent->modifiers());
		
		bool result = sendTabletEventRecursive(widget, &newEvent);
		event->setAccepted(result);
		
		return true;
	} else {
		return false;	// do not filter event
	}
}

bool FSTabletEventFilter::sendTabletEventRecursive(QWidget *widget, FSTabletEvent *event)
{
	QPoint delta = widget->mapFromGlobal(QPoint(0, 0));
	QPoint relativePos = event->globalPosInt + delta;
	if (!widget->rect().contains(relativePos))
		return false;
	
	event->data.pos = event->globalPos + MLVec2D(delta);
	event->setAccepted(false);
	QCoreApplication::sendEvent(widget, event);
	
	if (event->isAccepted())
		return true;
	
	foreach (QObject *child, widget->children()) {
		QWidget *childWidget = qobject_cast<QWidget *>(child);
		if (!childWidget)
			continue;
		
		if (childWidget->geometry().contains(relativePos)) {
			bool ret = sendTabletEventRecursive(childWidget, event);
			if (ret)
				return true;
		}
	}
	
	return false;
}
*/
