#ifndef APPLICATION_H
#define APPLICATION_H

#include "qtsingleapplication/qtsingleapplication.h"
#include <QTabletEvent>
#include "tabletpointerdata.h"

#ifdef Q_OS_MAC
#define PAINTFIELD_ENABLE_TABLET_EVENT_FILTER
#endif

namespace PaintField
{

class ApplicationEventFilter;

class Application : public QtSingleApplication
{
	Q_OBJECT
	Q_PROPERTY(bool tabletActive
	           READ isTabletActive
	           NOTIFY tabletActiveChanged)
	Q_PROPERTY(TabletPointerData tabletPointerData
	           READ tabletPointerData
	           NOTIFY tabletPointerChanged)
	
public:
	
	typedef QtSingleApplication super;
	
	Application(int &argc, char **argv);
	~Application();
	
	bool isTabletActive() const;
	TabletPointerData tabletPointerData() const;
	
signals:
	
	void tabletActivated();
	void tabletDeactivated();
	void tabletActiveChanged(bool active);
	
	void tabletPointerChanged(const TabletPointerData &pointerData);
	
	void fileOpenRequested(const QString &filepath);
	
public slots:
	
private slots:
	
	void onTabletEntered(QTabletEvent *ev);
	void onTabletLeft(QTabletEvent *ev);
	
private:
	
	class Data;
	Data *d;
};

}

#endif // APPLICATION_H
