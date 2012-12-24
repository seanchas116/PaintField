#ifndef APPLICATIONEVENTFILTER_H
#define APPLICATIONEVENTFILTER_H

#include <QWidget>
#include <QObject>
#include <QPointer>
#include <QTabletEvent>

#ifdef Q_OS_MAC
#define PAINTFIELD_ENABLE_TABLET_EVENT_FILTER
#endif

class QTabletEvent;

namespace PaintField
{

class WidgetTabletEvent;

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
	
	bool sendTabletEventToWindow(QWidget *window, QTabletEvent *event);
	void sendWidgetTabletEvent(WidgetTabletEvent *event, QWidget *widget);
	void setPrevWidget(QWidget *widget);
	
	QPointer<QWidget> _targetWidget;
	QPointer<QWidget> _lastPressedWidget;
	QPointer<QWidget> _prevWidget;
	bool _tabletStrokeAccepted = false;
	
#endif
};

}

#endif // APPLICATIONEVENTFILTER_H
