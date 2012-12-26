#ifndef APPLICATION_H
#define APPLICATION_H

#include "qtsingleapplication/qtsingleapplication.h"
#include <QTabletEvent>

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
	Q_PROPERTY(QTabletEvent::PointerType tabletPointerType
	           READ tabletPointerType
	           NOTIFY tabletPointerTypeChanged)
	Q_PROPERTY(quint64 tabletId
	           READ tabletId 
	           NOTIFY tabletIdChanged)
	
public:
	
	typedef QtSingleApplication super;
	
	Application(int &argc, char **argv);
	
	bool isTabletActive() const { return _isTabletActive; }
	QTabletEvent::PointerType tabletPointerType() const { return _tabletPointerType; }
	quint64 tabletId() const { return _tabletId; }
	
signals:
	
	void tabletActivated();
	void tabletDeactivated();
	void tabletActiveChanged(bool active);
	void tabletPointerTypeChanged(QTabletEvent::PointerType type);
	void tabletIdChanged(quint64 id);
	
	void fileOpenRequested(const QString &filepath);
	
public slots:
	
private slots:
	
	void onTabletEntered(QTabletEvent *ev);
	void onTabletLeft(QTabletEvent *ev);
	
private:
	
	bool _isTabletActive = false;
	QTabletEvent::PointerType _tabletPointerType = QTabletEvent::UnknownPointer;
	qint64 _tabletId = 0;
};

}

#endif // APPLICATION_H
