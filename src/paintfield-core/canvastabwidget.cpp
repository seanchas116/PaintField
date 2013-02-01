#include <QtGui>
#include "workspacecontroller.h"
#include "appcontroller.h"
#include "workspacemanager.h"
#include "canvasview.h"
#include "canvascontroller.h"

#include "canvastabwidget.h"

namespace PaintField {

struct CanvasTabWidget::Data
{
};

CanvasTabWidget::CanvasTabWidget(Workspace *workspace, QWidget *parent) :
    WorkspaceTabWidget(workspace, parent),
    d(new Data)
{
	new CanvasTabBar(this);
	
	setTabsClosable(true);
	setAutoDeletionEnabled(true);
	
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(activate()));
	connect(this, SIGNAL(tabClicked()), this, SLOT(activate()));
	connect(this, SIGNAL(tabMovedIn(QWidget*)), this, SLOT(activate()));
	
	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
	
	connect(this, SIGNAL(tabMovedIn(QWidget*)), this, SLOT(onTabMovedIn(QWidget*)));
	connect(this, SIGNAL(tabAboutToBeMovedOut(QWidget*)), this, SLOT(onTabAboutToBeMovedOut(QWidget*)));
	
	connect(workspace, SIGNAL(currentCanvasChanged(Canvas*)), this, SLOT(onCurrentCanvasChanged(Canvas*)));
	connect(this, SIGNAL(currentCanvasChanged(Canvas*)), workspace, SLOT(setCurrentCanvas(Canvas*)));
}

CanvasTabWidget::~CanvasTabWidget()
{
	delete d;
}

bool CanvasTabWidget::tabIsInsertable(DockTabWidget *other, int index)
{
	Q_UNUSED(index)
	
	CanvasTabWidget *tabWidget = qobject_cast<CanvasTabWidget *>(other);
	return tabWidget;
}

void CanvasTabWidget::insertTab(int index, QWidget *widget, const QString &title)
{
	Q_UNUSED(title)
	
	CanvasView *canvasView = qobject_cast<CanvasView *>(widget);
	if (!canvasView)
		return;
	
	insertCanvas(index, canvasView->canvas());
}

QObject *CanvasTabWidget::createNew()
{
	return new CanvasTabWidget(workspace(), 0);
}

void CanvasTabWidget::memorizeTransforms()
{
	for (auto view : canvasViews())
		view->memorizeNavigation();
}

void CanvasTabWidget::restoreTransforms()
{
	for (auto view : canvasViews())
		view->restoreNavigation();
}

void CanvasTabWidget::insertCanvas(int index, Canvas *canvas)
{
	workspace()->addCanvas(canvas);
	DockTabWidget::insertTab(index, canvas->view(), canvas->document()->fileName());
}

QList<CanvasView *> CanvasTabWidget::canvasViews()
{
	QList<CanvasView *> list;
	
	for (int i = 0; i < count(); ++i)
	{
		auto view = canvasViewAt(i);
		if (view)
			list << view;
	}
	
	return list;
}

void CanvasTabWidget::onCurrentCanvasChanged(Canvas *canvas)
{
	if (!canvas)
		return;
	
	bool set = false;
	
	for (int i = 0; i < count(); ++i)
	{
		if (canvas->view() == widget(i))
		{
			setCurrentIndex(i);
			set = true;
		}
	}
	
	if (set)
		activate();
}

void CanvasTabWidget::onTabMovedIn(QWidget *widget)
{
	CanvasView *canvasView = qobject_cast<CanvasView *>(widget);
	if (canvasView)
		canvasView->restoreNavigation();
}

void CanvasTabWidget::onTabAboutToBeMovedOut(QWidget *widget)
{
	CanvasView *canvasView = qobject_cast<CanvasView *>(widget);
	if (canvasView)
		canvasView->memorizeNavigation();
}

bool CanvasTabWidget::tryClose()
{
	for (auto canvasView : canvasViews())
	{
		if (!canvasView->canvas()->closeCanvas())
			return false;
	}
	
	return true;
}

void CanvasTabWidget::activate()
{
	auto canvasView = canvasViewAt(currentIndex());
	emit currentCanvasChanged(canvasView ? canvasView->canvas() : 0);
	emit activated();
}

void CanvasTabWidget::onTabCloseRequested(int index)
{
	auto canvasView = canvasViewAt(index);
	if (canvasView)
		canvasView->canvas()->closeCanvas();
}
CanvasView *CanvasTabWidget::canvasViewAt(int index)
{
	return qobject_cast<CanvasView *>(widget(index));
}

CanvasTabBar::CanvasTabBar(CanvasTabWidget *tabWidget, QWidget *parent) :
    DockTabBar(tabWidget, parent),
    _tabWidget(tabWidget)
{
	setAcceptDrops(true);
}

void CanvasTabBar::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls())
		event->acceptProposedAction();
}

void CanvasTabBar::dropEvent(QDropEvent *event)
{
	auto mimeData = event->mimeData();
	
	if (mimeData->hasUrls())
	{
		for (const QUrl &url : mimeData->urls())
		{
			auto canvas = Canvas::fromFile(url.toLocalFile());
			if (canvas)
			{
				new CanvasView(canvas);
				_tabWidget->insertCanvas(insertionIndexAt(event->pos()), canvas);
			}
		}
		event->acceptProposedAction();
	}
}


} // namespace PaintField
