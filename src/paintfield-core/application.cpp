#include "internal/applicationeventfilter.h"

#include "application.h"

namespace PaintField
{

Application::Application(int &argc, char **argv) :
	QtSingleApplication(argc, argv)
{
	auto filter = new ApplicationEventFilter(this);
	
	connect(filter, SIGNAL(tabletActivated()), this, SIGNAL(tabletActivated()));
	connect(filter, SIGNAL(tabletDeactivated()), this, SIGNAL(tabletDeactivated()));
	connect(filter, SIGNAL(tabletActiveChanged(bool)), this, SIGNAL(tabletActiveChanged(bool)));
	connect(filter, SIGNAL(tabletPointerTypeChanged(QTabletEvent::PointerType)), this, SIGNAL(tabletPointerTypeChanged(QTabletEvent::PointerType)));
	connect(filter, SIGNAL(tabletIdChanged(quint64)), this, SIGNAL(tabletIdChanged(quint64)));
	
	installEventFilter(new ApplicationEventFilter(this));
}

}
