#pragma once


#include <QObject>

namespace PaintField
{

class Test_RectLayer : public QObject
{
	Q_OBJECT
public:
	explicit Test_RectLayer(QObject *parent = 0);
	
signals:
	
public slots:
	
private slots:
	
	void test_encodeDecode();
	void test_saveProperties();
	void test_loadProperties();
	void test_setRect();
	
};

}

