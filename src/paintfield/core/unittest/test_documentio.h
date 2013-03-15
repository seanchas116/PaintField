#pragma once
#ifdef PF_TEST

#include <QObject>
#include "autotest.h"

namespace PaintField
{

class Test_DocumentIO : public QObject
{
	Q_OBJECT
public:
	explicit Test_DocumentIO(QObject *parent = 0);
	
signals:
	
public slots:
	
private slots:
	
	void saveLoad();
};

}

#endif // TEST_DOCUMENTIO_H
