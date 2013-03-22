#pragma once
#ifdef PF_TEST

#include <QObject>

namespace PaintField {

class Test_LayerIndex : public QObject
{
	Q_OBJECT
public:
	explicit Test_LayerIndex(QObject *parent = 0);
	
signals:
	
public slots:
	
private slots:
	
	void test();
	
};

} // namespace PaintField

#endif
