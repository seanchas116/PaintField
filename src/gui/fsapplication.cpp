#include <QtGui>

#include "fstableteventfilter.h"
#include "fsguimain.h"

#include "fsapplication.h"

FSApplication::FSApplication(int &argv, char **args) :
    QApplication(argv, args)
{
	installEventFilter(new FSTabletEventFilter(this));
	
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	
	new FSCore(this);
	new FSGuiMain(this);
}

bool FSApplication::event(QEvent *event)
{
	if (event->type() == QEvent::TabletEnterProximity || event->type() == QEvent::TabletLeaveProximity)
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
	
	return QApplication::event(event);
}
