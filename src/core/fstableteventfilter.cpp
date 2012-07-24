#include <QtGui>
#include "fstabletevent.h"

#include "fstableteventfilter.h"

bool FSTabletEventFilter::eventFilter(QObject *watched, QEvent *event)
{
	Q_UNUSED(watched);
	
	if (event->type() == QEvent::TabletMove || event->type() == QEvent::TabletPress || event->type() == QEvent::TabletRelease) {
		
		QElapsedTimer timer;
		timer.start();
		
		QTabletEvent *tabletEvent = static_cast<QTabletEvent *>(event);
		
		QWidget *widget = QApplication::topLevelAt(tabletEvent->globalPos());
		if (!widget) {
			event->ignore();
			return true;
		}
		
		FSGlobal::Event newEventType;
		switch (event->type()) {
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
		
		qDebug() << "tablet event handling took" << timer.elapsed() << "ms";
		
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
	
	event->data.pos = event->globalPos + QPointF(delta);
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
