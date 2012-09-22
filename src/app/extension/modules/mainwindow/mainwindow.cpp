#include <QtGui>
#include "QJson/Parser"

#include "canvasmdiarea.h"
#include "documenttabbar.h"

#include "../document/documentmodule.h"

#include "../action/actionmodule.h"

#include "mainwindow.h"

namespace PaintField
{

void applyMacSmallSize(QWidget *widget)
{
	widget->setAttribute(Qt::WA_MacSmallSize);
	foreach (QObject *object, widget->children())
	{
		QWidget *widget = qobject_cast<QWidget *>(object);
		if (widget)
			applyMacSmallSize(widget);
	}
}

PanelFrame::PanelFrame(QWidget *parent) :
	QWidget(parent),
    _panel(0)
{
	//setStyleSheet("FSPanel { background-color: rgb(250, 250, 250); }");
	
	//setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	setWindowFlags(Qt::Tool);
	
	_layout = new QVBoxLayout;
	_layout->setContentsMargins(8, 8, 8, 8);
	_layout->setSpacing(4);
	
	_label = new QLabel;
	
	//QFont font;
	//font.setBold(true);
	//_label->setFont(font);
	//_label->setAlignment(Qt::AlignHCenter);
	
	_layout->addWidget(_label);
}

void PanelFrame::setPanel(QWidget *panel)
{
	if (_panel)
	{
		_panel->deleteLater();
	}
	
	applyMacSmallSize(panel);
	
	_layout->addWidget(panel);
	_layout->addStretch(1);
	_label->setText(panel->windowTitle());
	_panel = panel;
	
	setLayout(_layout);
}

void PanelFrame::closeEvent(QCloseEvent *event)
{
	event->ignore();
	hide();
}

void PanelFrame::mousePressEvent(QMouseEvent *event)
{
	_origPanelPos = pos();
	_origMousePos = event->globalPos();
}

void PanelFrame::mouseMoveEvent(QMouseEvent *event)
{
	QPoint disp = event->globalPos() - _origMousePos;
	move(_origPanelPos + disp);
}

class DockWidget : public QDockWidget
{
	Q_OBJECT
	
public:
	
	DockWidget(QWidget *parent = 0) : QDockWidget(parent) {}
	
	void setPanelFrame(PanelFrame *frame)
	{
		setWidget(frame);
	}
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
	CanvasMdiArea *mdiArea = new CanvasMdiArea();
	
	DocumentTabBar *tabBar = new DocumentTabBar();
	
	QWidget *centralWidget = new QWidget();
	
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(tabBar);
	layout->addWidget(mdiArea);
	
	centralWidget->setLayout(layout);
	
	setCentralWidget(centralWidget);
	
	DocumentManager *dmanager = DocumentModule::documentManager();
	
	// connect with document manager
	
	connect(dmanager, SIGNAL(documentAdded(Document*)), mdiArea, SLOT(addDocument(Document*)));
	connect(dmanager, SIGNAL(documentRemoved(Document*)), mdiArea, SLOT(removeDocument(Document*)));
	connect(dmanager, SIGNAL(currentDocumentChanged(Document*)), mdiArea, SLOT(setCurrentDocument(Document*)));
	connect(dmanager, SIGNAL(documentVisibleChanged(Document*,bool)), mdiArea, SLOT(setDocumentVisible(Document*,bool)));
	
	connect(mdiArea, SIGNAL(currentDocumentChanged(Document*)), dmanager, SLOT(setCurrentDocument(Document*)));
	
	connect(dmanager, SIGNAL(documentAdded(Document*)), tabBar, SLOT(addDocument(Document*)));
	connect(dmanager, SIGNAL(documentRemoved(Document*)), tabBar, SLOT(removeDocument(Document*)));
	connect(dmanager, SIGNAL(currentDocumentChanged(Document*)), mdiArea, SLOT(setCurrentDocument(Document*)));
	
	connect(tabBar, SIGNAL(currentDocumentChanged(Document*)), dmanager, SLOT(setCurrentDocument(Document*)));
	connect(tabBar, SIGNAL(documentVisibleChanged(Document*,bool)), dmanager, SLOT(setDocumentVisible(Document*,bool)));
}

void MainWindow::arrangeMenus()
{
	ActionManager *actionManager = ActionModule::actionManager();
	
	actionManager->declareMenu("file", tr("File"));
	actionManager->declareMenu("file", tr("Edit"));
	actionManager->declareMenu("file", tr("Layer"));
	actionManager->declareMenu("file", tr("Window"));
	actionManager->declareMenu("file", tr("Help"));
	
	QJson::Parser parser;
	
	QFile file(":/mainmenu.json");
	
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << Q_FUNC_INFO << ": cannot load mainmenu.json";
		return;
	}
	
	QVariantList menuOrders = parser.parse(&file).toList();
	
	QMenuBar *menuBar = new QMenuBar();
	
	foreach (const QVariant &order, menuOrders)
	{
		QMenu *menu = new QMenu();
		actionManager->arrangeMenu(menu, order.toMap());
		menuBar->addMenu(menu);
	}
}

void MainWindow::arrangePanels()
{
	QFile file(":/panels.json");
	
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << Q_FUNC_INFO << ": cannot load panels.json";
		return;
	}
	
	QJson::Parser parser;
	
	QVariantMap panelOrder = parser.parse(&file).toMap();
	
	arrangePanelsInArea(Qt::LeftDockWidgetArea, panelOrder["left"].toList());
	arrangePanelsInArea(Qt::RightDockWidgetArea, panelOrder["right"].toList());
}

void MainWindow::arrangePanelsInArea(Qt::DockWidgetArea area, const QVariantList &list)
{
	foreach (const QVariant &item, list)
	{
		QString id = item.toString();
		if (id.isEmpty())
			continue;
		PanelFrame *frame = findPanelFrame(id);
		
		DockWidget *dockWidget = new DockWidget();
		dockWidget->setPanelFrame(frame);
		
		addDockWidget(area, dockWidget);
	}
}

void MainWindow::addPanel(QWidget *panel)
{
	PanelFrame *frame = new PanelFrame();
	frame->setPanel(panel);
	frame->setObjectName(panel->objectName());
}

PanelFrame *MainWindow::findPanelFrame(const QString &id)
{
	foreach (PanelFrame *frame, _panelFrames)
	{
		if (frame->objectName() == id)
			return frame;
	}
	return 0;
}

}
