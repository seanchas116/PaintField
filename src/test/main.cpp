#include <QtGui>
#include "autotest.h"
#include "test_documentio.h"
#include "testutil.h"
#include "paintfield-core/canvasview.h"
#include "paintfield-core/drawutil.h"
#include "paintfield-core/documentio.h"

using namespace Malachite;

void test_CanvasView()
{
	auto view = new CanvasView(TestUtil::createTestDocument(), 0);
	view->show();
}

void test_Surface()
{
	Surface surface = TestUtil::createTestSurface(0);
	
	QPixmap pixmap(400, 300);
	
	QPainter painter(&pixmap);
	drawMLSurface(&painter, 0, 0, surface);
	
	auto label = new QLabel;
	label->setWindowTitle("test_Surface");
	label->setPixmap(pixmap);
	label->show();
}

void test_DocumentIO_saveLoad()
{
	const QString path = "/Users/iofg2100/Desktop/test/test.pfield";
	
	auto doc = TestUtil::createTestDocument(0);
	
	{
		DocumentIO documentIO;
		documentIO.saveAs(doc, path);
	}
	
	Document *openedDoc;
	{
		DocumentIO documentIO(path);
		openedDoc = documentIO.load(0);
	}
	
	auto view = new CanvasView(openedDoc, 0);
	view->show();
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	//autoTest.run();
	
	
	test_DocumentIO_saveLoad();
	//test_Surface();
	//test_CanvasView();
	
	return app.exec();
}
