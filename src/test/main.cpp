#include <QtCore>
#include "autotest.h"
#include "testutil.h"
#include "paintfield-core/canvasview.h"

void test_CanvasView()
{
	CanvasView view(TestUtil::createTestDocument(), 0);
	view.show();
}

int main(int argc, char *argv[])
{
	int result = autoTest.run();
	
	if (result == 0)
		qDebug() << "AUTO TEST PASSED";
	else
		qDebug() << "AUTO TEST FAILED!";
	
	QApplication app(argc, argv);
	
	test_CanvasView();
	
	return app.exec();
}
