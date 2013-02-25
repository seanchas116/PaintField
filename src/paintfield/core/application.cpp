#include "internal/applicationeventfilter.h"

#include "application.h"

namespace PaintField
{

struct Application::Data
{
	bool isTabletActive = false;
	TabletPointerInfo tabletPointerInfo;
};

Application::Application(int &argc, char **argv) :
	QtSingleApplication(argc, argv),
    d(new Data)
{
	auto filter = new ApplicationEventFilter(this);
	
	connect(filter, SIGNAL(tabletEntered(QTabletEvent*)), this, SLOT(onTabletEntered(QTabletEvent*)));
	connect(filter, SIGNAL(tabletLeft(QTabletEvent*)), this, SLOT(onTabletLeft(QTabletEvent*)));
	
	connect(filter, SIGNAL(fileOpenRequested(QString)), this, SIGNAL(fileOpenRequested(QString)));
	
	connect(this, SIGNAL(messageReceived(QString)), this, SIGNAL(fileOpenRequested(QString)));
	
	installEventFilter(filter);
}

Application::~Application()
{
	delete d;
}

bool Application::isTabletActive() const
{
	return d->isTabletActive;
}

TabletPointerInfo Application::tabletPointerData() const
{
	return d->tabletPointerInfo;
}

void Application::onTabletEntered(QTabletEvent *ev)
{
	d->isTabletActive = true;
	emit tabletActiveChanged(true);
	emit tabletActivated();
	
	TabletPointerInfo pointerInfo(ev->uniqueId(), ev->pointerType());
	if (d->tabletPointerInfo != pointerInfo)
	{
		auto prev = d->tabletPointerInfo;
		d->tabletPointerInfo = pointerInfo;
		emit tabletPointerChanged(pointerInfo, prev);
	}
}

void Application::onTabletLeft(QTabletEvent *ev)
{
	Q_UNUSED(ev)
	d->isTabletActive = false;
	emit tabletActiveChanged(false);
	emit tabletDeactivated();
}

}
