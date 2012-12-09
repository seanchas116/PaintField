#include <QtGui>
#include "autotest.h"
#include "test_documentio.h"
#include "testutil.h"
#include "testobject.h"

#include "splittabareatestwidget.h"

#include "paintfield-core/debug.h"
#include "paintfield-core/canvascontroller.h"
#include "paintfield-core/canvasview.h"
#include "paintfield-core/drawutil.h"
#include "paintfield-core/documentio.h"
#include "paintfield-core/widgets/docktabmotherwidget.h"
#include "paintfield-core/widgets/splittabareacontroller.h"
#include "paintfield-core/widgets/tabwidget.h"
#include "paintfield-core/widgets/vanishingscrollbar.h"

#include "paintfield-extension/navigator/navigatorview.h"

using namespace Malachite;


void test_CanvasView()
{
    auto controller = new CanvasController(TestUtil::createTestDocument(), 0);
    auto view = new CanvasView(controller);
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
	
    auto controller = new CanvasController(openedDoc, 0);
    auto view = new CanvasView(controller);
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
	
	auto tabWidget = new FloatingDockTabWidget(motherWidget, 0);
	tabWidget->addTab(new QLabel("Tab 1"), "Tab 1");
	tabWidget->addTab(new QLabel("Tab 2"), "Tab 2");
	tabWidget->addTab(new QLabel("Tab 3"), "Tab 3");
	
	tabWidget->show();
	tabWidget->setFocus();
}

/*
void test_TabDocumentAreaNode()
{
	auto node = new TabDocumentAreaNode(new DockTabWidget);
	node->insert(TabDocumentAreaNode::First, Qt::Horizontal);
	
	node->splitter()->show();
	node->children().at(1)->insert(TabDocumentAreaNode::Second, Qt::Vertical);
	//node->close(TabDocumentAreaNode::First);
}*/

void test_TabWidget()
{
	auto tabWidget = new TabWidget();
	tabWidget->addTab(new QLabel("Tab 1"), "Tab 1");
	tabWidget->addTab(new QLabel("Tab 2"), "Tab 2");
	tabWidget->addTab(new QLabel("Tab 3"), "Tab 3");
	
	tabWidget->takeTab(0);
	tabWidget->takeTab(0);
	tabWidget->takeTab(0);
	
	tabWidget->resize(500, 500);
	tabWidget->show();
	
	auto tabBar = new QTabBar;
	tabBar->resize(500, 500);
	tabBar->show();
}

void test_SplitTabAreaController()
{
	auto testWidget = new SplitTabAreaTestWidget;
	testWidget->show();
	
	auto widget = new QWidget;
	widget->setFocusPolicy(Qt::ClickFocus);
	widget->show();
}

void test_scrollarea()
{
	auto abstractScrollArea = new QAbstractScrollArea;
	
	auto label = new QLabel("Label");
	
	abstractScrollArea->setViewport(label);
	label->setVisible(true);
	abstractScrollArea->show();
}

void test_VanishingScrollBar()
{
	auto createScrollBar = [](Qt::Orientation orientation)
	{
		auto bar = new VanishingScrollBar(orientation);
		bar->setMinimum(0);
		bar->setMaximum(100);
		bar->setPageStep(25);
		return bar;
	};
	
	createScrollBar(Qt::Horizontal)->show();
	createScrollBar(Qt::Vertical)->show();
}

void test_VanishingScrollBar_vanishing()
{
	auto widget = new QWidget;
	auto layout = new QHBoxLayout;
	
	auto bar = new VanishingScrollBar(Qt::Vertical);
	bar->setMinimum(0);
	bar->setMaximum(100);
	bar->setPageStep(25);
	
	auto button = new QPushButton("press");
	
	QObject::connect(button, SIGNAL(pressed()), bar, SLOT(wakeUp()));
	
	layout->addWidget(bar);
	layout->addWidget(button);
	widget->setLayout(layout);
	
	widget->show();
}

void test_NavigatorView()
{
	auto view = new NavigatorView;
	applyMacSmallSize(view);
	view->show();
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	//autoTest.run();
	
	//auto testObj = new TestObject;
	//QObject::connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), testObj, SLOT(onFocusChanged(QWidget*,QWidget*)));
	
	//test_SplitTabAreaController();
	//test_TabWidget();
	//test_TabDocumentAreaNode();
	//test_DockTabWidget();
	//test_DocumentIO_saveLoad();
	//test_Surface();
	//test_CanvasView();
	//test_scrollarea();
	//test_VanishingScrollBar();
	test_VanishingScrollBar_vanishing();
	//test_NavigatorView();
	
	return app.exec();
}
