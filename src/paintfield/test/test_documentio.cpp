#include <QtCore>
#include "paintfield/core/documentio.h"

#include "testutil.h"
#include "test_documentio.h"

Test_DocumentIO::Test_DocumentIO(QObject *parent) :
	QObject(parent)
{
}

void Test_DocumentIO::saveLoad()
{
	auto tempDir = TestUtil::createTestDir();
	auto path = tempDir.filePath("test.pfield");
	
	auto doc = TestUtil::createTestDocument(this);
	
	{
		DocumentSaver saver(doc);
		saver.save(path);
	}
	
	Document *openedDoc;
	{
		DocumentLoader loader;
		openedDoc = loader.load(path, 0);
	}
	
	QCOMPARE(doc->layerModel()->rootLayer()->count(), openedDoc->layerModel()->rootLayer()->count());
}

AUTOTEST_ADD_CLASS(Test_DocumentIO)
