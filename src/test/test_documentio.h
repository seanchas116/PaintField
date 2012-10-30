#ifndef TEST_DOCUMENTIO_H
#define TEST_DOCUMENTIO_H

#include <QObject>
#include "autotest.h"

class Test_DocumentIO : public QObject
{
	Q_OBJECT
public:
	explicit Test_DocumentIO(QObject *parent = 0);
	
signals:
	
public slots:
	
	void saveLoad();
	
private slots:
};



#endif // TEST_DOCUMENTIO_H
