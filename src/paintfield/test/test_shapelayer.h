#ifndef TEST_SHAPELAYER_H
#define TEST_SHAPELAYER_H

#include <QObject>
#include "autotest.h"

class Test_ShapeLayer : public QObject
{
	Q_OBJECT
public:
	explicit Test_ShapeLayer(QObject *parent = 0);
	
signals:
	
public slots:
	
private slots:
	
	void test_encodeDecode();
	void test_saveProperties();
	void test_loadProperties();
	
	void test_setFillPath();
};

#endif // TEST_SHAPELAYER_H
