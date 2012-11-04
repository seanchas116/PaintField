#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QObject>
#include <QDebug>

class TestObject : public QObject
{
	Q_OBJECT
public:
	explicit TestObject(QObject *parent = 0) : QObject(parent) {}
	
signals:
	
public slots:
	
	void onFocusChanged(QWidget *old, QWidget *now)
	{
		qDebug() << "focus changed";
	}
};

#endif // TESTOBJECT_H
