#ifdef PF_TEST

#include <QtCore>
#include "paintfield/core/documentio.h"

#include "testutil.h"
#include "test_documentio.h"

namespace PaintField
{

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
	
	QCOMPARE(doc->layerModel()->rootLayer()->childCount(), openedDoc->layerModel()->rootLayer()->childCount());
}

PF_ADD_TESTCLASS(Test_DocumentIO)

}

#endif
