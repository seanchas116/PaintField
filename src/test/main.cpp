#include <QtGui>
#include "autotest.h"
#include "test_documentio.h"
#include "testutil.h"
#include "testobject.h"

#include "paintfield-core/canvasview.h"
#include "paintfield-core/drawutil.h"
#include "paintfield-core/documentio.h"
#include "paintfield-core/widgets/docktabmotherwidget.h"
#include "paintfield-core/widgets/tabdocumentarea.h"

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

void test_DockTabWidget()
{
	auto widget = new QWidget;
	widget->resize(400, 300);
	widget->setWindowTitle("Normal Widget");
	widget->show();
	widget->setFocus();
	
	auto motherWidget = new DockTabMotherWidget;
	motherWidget->resize(400, 300);
	motherWidget->setWindowTitle("Mother Widget");
	motherWidget->show();
	motherWidget->setFocus();
	
	auto tabWidget = new DockTabWidget;
	tabWidget->addTab(new QLabel("Tab 1"), "Tab 1");
	tabWidget->addTab(new QLabel("Tab 2"), "Tab 1");
	tabWidget->addTab(new QLabel("Tab 3"), "Tab 1");
	
	tabWidget->show();
	tabWidget->setFocus();
}

void test_TabDocumentAreaNode()
{
	auto node = new TabDocumentAreaNode(new DockTabWidget);
	node->insert(TabDocumentAreaNode::First, Qt::Horizontal);
	
	node->splitter()->show();
	node->children().at(1)->insert(TabDocumentAreaNode::Second, Qt::Vertical);
	//node->close(TabDocumentAreaNode::First);
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	//autoTest.run();
	
	//auto testObj = new TestObject;
	//QObject::connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), testObj, SLOT(onFocusChanged(QWidget*,QWidget*)));
	
	test_TabDocumentAreaNode();
	//test_DockTabWidget();
	//test_DocumentIO_saveLoad();
	//test_Surface();
	//test_CanvasView();
	
	return app.exec();
}
