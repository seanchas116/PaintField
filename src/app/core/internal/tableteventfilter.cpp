#include <QtGui>
#include "core/tabletevent.h"

#include "tableteventfilter.h"

namespace PaintField
{

bool TabletEventFilter::eventFilter(QObject *watched, QEvent *event)
{
	Q_UNUSED(watched);
	
	if (event->type() == QEvent::TabletMove || event->type() == QEvent::TabletPress || event->type() == QEvent::TabletRelease)
	{
		QTabletEvent *tabletEvent = static_cast<QTabletEvent *>(event);
		
		QWidget *widget = _targetWidget ? _targetWidget : QApplication::widgetAt(tabletEvent->globalPos());
		
		if (!widget)
			return true;
		
		PaintField::Event newEventType;
		switch (event->type())
		{
		case QEvent::TabletMove:
			newEventType = PaintField::EventTabletMove;
			break;
		case QEvent::TabletPress:
			newEventType = PaintField::EventTabletPress;
			break;
		case QEvent::TabletRelease:
			newEventType = PaintField::EventTabletRelease;
			break;
		default:
			event->ignore();
			return true;
		}
		
		TabletEvent newEvent(newEventType,
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

}
