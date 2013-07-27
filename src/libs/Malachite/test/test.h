#ifndef TEST_H
#define TEST_H

#include <QObject>

class Test : public QObject
{
	Q_OBJECT
public:
	explicit Test(QObject *parent = 0);
	
signals:
	
public slots:
	
private slots:
	
	void test_blend();
};

#endif // TEST_H
