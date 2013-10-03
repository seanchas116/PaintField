#pragma once

#include <QWidget>
#include <QObject>
#include <QPointer>
#include <QTabletEvent>

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
};

}

