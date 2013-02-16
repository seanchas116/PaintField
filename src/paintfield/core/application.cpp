#include "internal/applicationeventfilter.h"

#include "application.h"

namespace PaintField
{

struct Application::Data
{
	bool isTabletActive = false;
	TabletPointerData tabletPointerData;
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

TabletPointerData Application::tabletPointerData() const
{
	return d->tabletPointerData;
}

void Application::onTabletEntered(QTabletEvent *ev)
{
	d->isTabletActive = true;
	emit tabletActiveChanged(true);
	emit tabletActivated();
	
	TabletPointerData pointerData(ev->uniqueId(), ev->pointerType());
	if (d->tabletPointerData != pointerData)
	{
		d->tabletPointerData = pointerData;
		emit tabletPointerChanged(pointerData);
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
