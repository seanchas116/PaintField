#include <QtGui>
#include "autotest.h"
#include "testutil.h"
#include "paintfield-core/canvasview.h"
#include "paintfield-core/drawutil.h"

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

int main(int argc, char *argv[])
{
	int result = autoTest.run();
	
	if (result == 0)
		qDebug() << "AUTO TEST PASSED";
	else
		qDebug() << "AUTO TEST FAILED!";
	
	QApplication app(argc, argv);
	
	test_Surface();
	test_CanvasView();
	
	return app.exec();
}
