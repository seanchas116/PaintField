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
	
signals:
	
	void tabletEntered(QTabletEvent *ev);
	void tabletLeft(QTabletEvent *ev);
	
	void fileOpenRequested(const QString &path);
	
public slots:
	
private:
	
#ifdef PAINTFIELD_ENABLE_TABLET_EVENT_FILTER
	
	bool sendTabletEvent(QWidget *window, QTabletEvent *event);
	QWidget *_trackedWindow = 0;
	bool _tabletEventAcceptedInTargetWindow = false;
	
#endif
};

}

#endif // APPLICATIONEVENTFILTER_H
