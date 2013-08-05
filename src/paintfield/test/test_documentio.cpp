
#include <QtCore>
#include "paintfield/core/document.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/paintfieldformatsupport.h"

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
	qDebug() << path;
	
	auto formatSupport = new PaintFieldFormatSupport(this);
	
	auto doc = TestUtil::createTestDocument(this);
	FormatSupport::exportToFile(path, formatSupport, doc->layerScene()->rootLayer()->children(), doc->size(), QVariant());
	
	Document *openedDoc;
	
	{
		QSize size;
		QList<LayerRef> layers;
		QString name;
		
		FormatSupport::importFromFile(path, {formatSupport}, &layers, &size, &name);
		
		openedDoc = new Document(name, size, layers, this);
	}
	
	QCOMPARE(doc->layerScene()->rootLayer()->count(), openedDoc->layerScene()->rootLayer()->count());
}

PF_ADD_TESTCLASS(Test_DocumentIO)

}

