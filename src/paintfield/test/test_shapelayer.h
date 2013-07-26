#pragma once


#include <QObject>

namespace PaintField
{

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

}
