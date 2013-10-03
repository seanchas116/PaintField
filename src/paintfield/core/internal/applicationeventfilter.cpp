#include <QApplication>

#include "applicationeventfilter.h"

namespace PaintField
{

ApplicationEventFilter::ApplicationEventFilter(QObject *parent) :
    QObject(parent)
{}

bool ApplicationEventFilter::eventFilter(QObject *watched, QEvent *event)
{
	Q_UNUSED(watched);
	switch (event->type())
	{
		default:
			return false;

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

}
