#include "internal/applicationeventfilter.h"

#include "application.h"

namespace PaintField
{

Application::Application(int &argc, char **argv) :
	QtSingleApplication(argc, argv)
{
	auto filter = new ApplicationEventFilter(this);
	
	connect(filter, SIGNAL(tabletEntered(QTabletEvent*)), this, SLOT(onTabletEntered(QTabletEvent*)));
	connect(filter, SIGNAL(tabletLeft(QTabletEvent*)), this, SLOT(onTabletLeft(QTabletEvent*)));
	
	installEventFilter(filter);
}

void Application::onTabletEntered(QTabletEvent *ev)
{
	_isTabletActive = true;
	emit tabletActiveChanged(true);
	emit tabletActivated();
	
	if (_tabletPointerType != ev->pointerType())
	{
		_tabletPointerType = ev->pointerType();
		emit tabletPointerTypeChanged(_tabletPointerType);
	}
	
	if (_tabletId != ev->uniqueId())
	{
		_tabletId = ev->uniqueId();
		emit tabletIdChanged(_tabletId);
	}
}

void Application::onTabletLeft(QTabletEvent *ev)
{
	Q_UNUSED(ev)
	_isTabletActive = false;
	emit tabletActiveChanged(false);
	emit tabletDeactivated();
}

}
