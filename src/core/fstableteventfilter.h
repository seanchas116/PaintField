#ifndef FSTABLETEVENTFILTER_H
#define FSTABLETEVENTFILTER_H

#include <QObject>

class QTabletEvent;
class FSTabletEvent;

// avoid the use of QTabletEvent events by the widgets and issue original tablet events,
// because Qt's tablet event system seems to have bugs
class FSTabletEventFilter : public QObject
{
	Q_OBJECT
public:
	explicit FSTabletEventFilter(QObject *parent = 0) : QObject(parent), _targetWidget(0) {}
	
	bool eventFilter(QObject *watched, QEvent *event);
	
signals:
	
public slots:
	
private:
	//bool sendTabletEventRecursive(QWidget *widget, FSTabletEvent *event);
	
	QWidget *_targetWidget;
};

#endif // FSTABLETEVENTFILTER_H
