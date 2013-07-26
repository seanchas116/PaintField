#pragma once


#include "testutil.h"
#include "paintfield/core/librarymodel.h"
#include <QObject>

namespace PaintField
{

class Test_LibraryModel : public QObject
{
	Q_OBJECT
public:
	explicit Test_LibraryModel(QObject *parent = 0);
	
	static QString createTestDirStructure();
	
signals:
	
public slots:
	
private slots:
	
	void initTestCase();
	void checkStructure();
	void pathForItem();
	void updateDirItemRoot();
	void updateDirItem();
	
private:
	
	LibraryModel *_model;
	QString _rootPath;
};

}

