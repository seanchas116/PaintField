#ifndef FSTABLETEVENTFILTER_H
#define FSTABLETEVENTFILTER_H

#include <QObject>

class QTabletEvent;

namespace PaintField {

class CanvasTabletEvent;

// avoid the use of QTabletEvent events by the widgets and issue original tablet events,
// because Qt's tablet event system seems to have bugs
class TabletEventFilter : public QObject
{
	Q_OBJECT
public:
	explicit TabletEventFilter(QObject *parent = 0) : QObject(parent), _targetWindow(0) {}
	
	bool eventFilter(QObject *watched, QEvent *event);
	
signals:
	
public slots:
	
private:
	
	bool sendTabletEvent(QWidget *window, QTabletEvent *event);
	
	QWidget *_targetWindow;
};

}

#endif // FSTABLETEVENTFILTER_H
