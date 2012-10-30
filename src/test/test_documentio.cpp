#include "paintfield-core/documentio.h"

#include "testutil.h"
#include "test_documentio.h"

Test_DocumentIO::Test_DocumentIO(QObject *parent) :
	QObject(parent)
{
}

void Test_DocumentIO::saveLoad()
{
	const QString path = "/Users/iofg2100/Desktop/test/test.pfield";
	
	Document *doc = TestUtil::createTestDocument(this);
	
	{
		DocumentIO documentIO;
		documentIO.saveAs(doc, path);
	}
	
	Document *openedDoc;
	{
		DocumentIO documentIO(path);
		openedDoc = documentIO.load(this);
	}
}
