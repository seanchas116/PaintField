
#include <QtCore>
#include "paintfield/core/documentio.h"
#include "paintfield/core/layerscene.h"

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
		DocumentWriter saver(doc);
		saver.save(path);
	}
	
	Document *openedDoc;
	{
		DocumentReader loader;
		openedDoc = loader.load(path, 0);
	}
	
	QCOMPARE(doc->layerScene()->rootLayer()->count(), openedDoc->layerScene()->rootLayer()->count());
}

PF_ADD_TESTCLASS(Test_DocumentIO)

}

