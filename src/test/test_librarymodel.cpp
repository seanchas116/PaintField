#include <QtGui>
#include "paintfield-core/librarymodel.h"

#include "test_librarymodel.h"

Test_LibraryModel::Test_LibraryModel(QObject *parent) :
    QObject(parent)
{
}

QString Test_LibraryModel::createTestDirStructure()
{
	auto tempDir = QDir(TestUtil::createTestDir());
	
	TestUtil::createTestFile(tempDir.filePath("0.txt"));
	
	tempDir.mkdir("1");
	{
		auto dir1 = tempDir;
		dir1.cd("1");
		
		TestUtil::createTestFile(dir1.filePath("10.txt"));
		
		dir1.mkdir("11");
		
		{
			auto dir11 = dir1;
			dir11.cd("11");
			
			TestUtil::createTestFile(dir11.filePath("110.txt"));
		}
		
	}
	
	qDebug() << tempDir.entryList();
	
	qDebug() << tempDir.path();
	return tempDir.path();
}

void Test_LibraryModel::initTestCase()
{
	_model = new LibraryModel;
	_rootPath = createTestDirStructure();
	_model->addRootPath(_rootPath, "test");
}

void Test_LibraryModel::checkStructure()
{
	QCOMPARE(_model->rowCount(), 1);
	QCOMPARE(_model->item(0)->text(), QString("test"));
	QCOMPARE(_model->item(0)->rowCount(), 2);
	QCOMPARE(_model->item(0)->child(0)->text(), QString("0.txt"));
	QCOMPARE(_model->item(0)->child(1)->text(), QString("1"));
	QCOMPARE(_model->item(0)->child(1)->rowCount(), 2);
	QCOMPARE(_model->item(0)->child(1)->child(0)->text(), QString("10.txt"));
	QCOMPARE(_model->item(0)->child(1)->child(1)->text(), QString("11"));
	QCOMPARE(_model->item(0)->child(1)->child(1)->rowCount(), 1);
	QCOMPARE(_model->item(0)->child(1)->child(1)->child(0)->text(), QString("110.txt"));
}

void Test_LibraryModel::pathForItem()
{
	auto path = _model->pathFromItem(_model->item(0)->child(1)->child(1)->child(0));
	auto expectedPath =_rootPath + "/1/11/110.txt";
	
	QCOMPARE(path, expectedPath);
}

void Test_LibraryModel::updateDirItemRoot()
{
	auto item = _model->item(0);
	auto dir = QDir(_rootPath);
	TestUtil::createTestFile(dir.filePath("2.txt"));
	_model->updateDirItem(item);
	
	QCOMPARE(_model->item(0)->rowCount(), 3);
	QCOMPARE(_model->item(0)->child(0)->text(), QString("0.txt"));
	QCOMPARE(_model->item(0)->child(1)->text(), QString("1"));
	QCOMPARE(_model->item(0)->child(2)->text(), QString("2.txt"));
}

void Test_LibraryModel::updateDirItem()
{
	auto item = _model->item(0)->child(1);
	auto dir = QDir(_rootPath);
	dir.cd("1");
	TestUtil::createTestFile(dir.filePath("12.txt"));
	
	_model->updateDirItem(item);
	
	QCOMPARE(_model->item(0)->child(1)->rowCount(), 3);
	QCOMPARE(_model->item(0)->child(1)->child(0)->text(), QString("10.txt"));
	QCOMPARE(_model->item(0)->child(1)->child(1)->text(), QString("11"));
	QCOMPARE(_model->item(0)->child(1)->child(2)->text(), QString("12.txt"));
}

AUTOTEST_ADD_CLASS(Test_LibraryModel)
