#pragma once

#ifdef PF_TEST

#include <QObject>
#include "autotest.h"
#include "paintfield/core/document.h"

namespace PaintField
{

class Test_Document : public QObject
{
	Q_OBJECT
public:
	explicit Test_Document(QObject *parent = 0);
	
signals:
	
public slots:
	
};

}

#endif
