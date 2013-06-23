#include <QUrl>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "workspace.h"
#include "appcontroller.h"
#include "workspacemanager.h"
#include "canvasview.h"
#include "canvas.h"
#include "documentcontroller.h"

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
	
	connect(workspace, SIGNAL(canvasDocumentPropertyChanged(Canvas*)), this, SLOT(onCanvasDocumentPropertyChanged(Canvas*)));
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
		view->canvas()->memorizeNavigation();
}

void CanvasTabWidget::restoreTransforms()
{
	for (auto view : canvasViews())
		view->canvas()->restoreNavigation();
}

static QString canvasTabText(Canvas *canvas)
{
	QString text = canvas->document()->fileName();
	if (canvas->document()->isModified())
	{
		text += "*";
	}
	return text;
}

void CanvasTabWidget::insertCanvas(int index, Canvas *canvas)
{
	if (canvas->workspace() != workspace())
	{
		auto originalCanvas = canvas;
		canvas = new Canvas(canvas, workspace());
		originalCanvas->closeCanvas();
	}
	
	canvas->memorizeNavigation();
	
	workspace()->addCanvas(canvas);
	if (!canvas->viewController())
		new CanvasViewController(canvas);
	
	DockTabWidget::insertTab(index, canvas->view(), canvasTabText(canvas));
	
	canvas->restoreNavigation();
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
		canvasView->canvas()->restoreNavigation();
}

void CanvasTabWidget::onTabAboutToBeMovedOut(QWidget *widget)
{
	CanvasView *canvasView = qobject_cast<CanvasView *>(widget);
	if (canvasView)
		canvasView->canvas()->memorizeNavigation();
}

void CanvasTabWidget::onCanvasDocumentPropertyChanged(Canvas *canvas)
{
	for (int i = 0; i < count(); ++i)
	{
		auto view = canvasViewAt(i);
		if (view && view->canvas() == canvas)
		{
			setTabText(i, canvasTabText(canvas));
		}
	}
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

void CanvasTabWidget::addCanvasesFromUrls(const QList<QUrl> &urls)
{
	for (const QUrl &url : urls)
	{
		auto filepath = url.toLocalFile();
		auto existingCanvas = appController()->findCanvasWithFilepath(filepath);
		
		Canvas *canvas = nullptr;
		
		if (existingCanvas)
		{
			canvas = new Canvas(existingCanvas, workspace());
		}
		else
		{
			auto document = DocumentController::createFromFile(url.toLocalFile());
			
			if (document)
				canvas = new Canvas(document, workspace());
			else
				return;
		}
		
		addCanvas(canvas);
	}
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
		_tabWidget->addCanvasesFromUrls(mimeData->urls());
		event->acceptProposedAction();
	}
}



} // namespace PaintField
