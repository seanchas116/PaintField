#ifndef APPLICATIONEVENTFILTER_H
#define APPLICATIONEVENTFILTER_H

#include <QObject>
#include <QTabletEvent>

#ifdef Q_OS_MAC
#define PAINTFIELD_ENABLE_TABLET_EVENT_FILTER
#endif

class QTabletEvent;

namespace PaintField
{

class CanvasTabletEvent;

class ApplicationEventFilter : public QObject
{
	Q_OBJECT
public:
	explicit ApplicationEventFilter(QObject *parent = 0);
	
	bool eventFilter(QObject *watched, QEvent *event);
	
	bool isTabletActive() const { return _isTabletActive; }
	QTabletEvent::PointerType tabletPointerType() const { return _tabletPointerType; }
	quint64 tabletId() const { return _tabletId; }
	
signals:
	
	void tabletActivated();
	void tabletDeactivated();
	void tabletActiveChanged(bool active);
	void tabletPointerTypeChanged(QTabletEvent::PointerType type);
	void tabletIdChanged(quint64 id);
	
	void fileOpenRequested(const QString &path);
	
public slots:
	
private:
	
#ifdef PAINTFIELD_ENABLE_TABLET_EVENT_FILTER
	bool sendTabletEvent(QWidget *window, QTabletEvent *event);
	QWidget *_targetWindow = 0;
#endif
	bool _isTabletActive = false;
	QTabletEvent::PointerType _tabletPointerType = QTabletEvent::UnknownPointer;
	qint64 _tabletId = 0;
};

}

#endif // APPLICATIONEVENTFILTER_H
