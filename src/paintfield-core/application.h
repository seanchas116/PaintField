#ifndef APPLICATION_H
#define APPLICATION_H

#include "qtsingleapplication/qtsingleapplication.h"
#include <QTabletEvent>

#ifdef Q_OS_MAC
#define PAINTFIELD_ENABLE_TABLET_EVENT_FILTER
#endif

namespace PaintField
{

class Application : public QtSingleApplication
{
	Q_OBJECT
public:
	
	typedef QtSingleApplication super;
	
	Application(int &argc, char **argv);
	
signals:
	
	void tabletActivated();
	void tabletDeactivated();
	void tabletActiveChanged(bool active);
	void tabletPointerTypeChanged(QTabletEvent::PointerType type);
	void tabletIdChanged(quint64 id);
	
public slots:
	
protected:
	
private:
	
};

}

#endif // APPLICATION_H
