#pragma once

#include <QObject>

namespace PaintField
{

class Test_ZipUnzip : public QObject
{
	Q_OBJECT
public:
	explicit Test_ZipUnzip(QObject *parent = 0);
	
signals:
	
public slots:
	
private slots:
	
	void zipUnzip();
};

}
