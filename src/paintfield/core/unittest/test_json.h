#pragma once
#ifdef PF_TEST

#include <QObject>

namespace PaintField
{

class Test_Json : public QObject
{
	Q_OBJECT
public:
	explicit Test_Json(QObject *parent = 0);
	
private slots:
	
	void test_readWrite();
};

}

#endif
