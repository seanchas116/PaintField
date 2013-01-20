#include <QUrl>
#include "workspacecontroller.h"
#include "debug.h"

#include "canvastabwidget.h"

namespace PaintField {

struct CanvasTabWidgetData
{
	WorkspaceController *workspace;
};

CanvasTabWidget::CanvasTabWidget(WorkspaceView *workspaceView, QWidget *parent) :
    FloatingDockTabWidget(workspaceView, parent),
    d(new CanvasTabWidgetData)
{
	d->workspace = workspaceView->controller();
	commonInit();
}

CanvasTabWidget::CanvasTabWidget(CanvasTabWidget *other, QWidget *parent) :
    FloatingDockTabWidget(other, parent),
    d(new CanvasTabWidgetData)
{
	*d = *other->d;
	commonInit();
}

CanvasTabWidget::~CanvasTabWidget()
{
	delete d;
}

void CanvasTabWidget::commonInit()
{
	new CanvasTabBar(this);
	
	setTabsClosable(true);
	
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(activate()));
	connect(this, SIGNAL(tabClicked()), this, SIGNAL(activated()));
	connect(this, SIGNAL(tabMovedIn(QWidget*)), this, SIGNAL(activated()));
	
	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
	
	connect(d->workspace, SIGNAL(currentCanvasChanged(CanvasController*)), this, SLOT(setCurrentCanvas(CanvasController*)));
	connect(this, SIGNAL(currentCanvasChanged(CanvasController*)), d->workspace, SLOT(setCurrentCanvas(CanvasController*)));
}

bool CanvasTabWidget::tabIsInsertable(DockTabWidget *other, int index)
{
	Q_UNUSED(index)
	
	CanvasTabWidget *tabWidget = qobject_cast<CanvasTabWidget *>(other);
	return tabWidget && tabWidget->d->workspace == d->workspace;
}

QObject *CanvasTabWidget::createNew()
{
	return new CanvasTabWidget(this, 0);
}

void CanvasTabWidget::memorizeTransforms()
{
	for (auto view : canvasViews())
		view->memorizeTransform();
}

void CanvasTabWidget::restoreTransforms()
{
	for (auto view : canvasViews())
		view->restoreTransform();
}

void CanvasTabWidget::insertCanvas(int index, CanvasController *canvas)
{
	insertTab(index, canvas->view(), canvas->document()->fileName());
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

WorkspaceController *CanvasTabWidget::workspace()
{
	return d->workspace;
}

void CanvasTabWidget::setCurrentCanvas(CanvasController *canvas)
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

bool CanvasTabWidget::tryClose()
{
	for (auto canvasView : canvasViews())
	{
		if (!canvasView->controller()->closeCanvas())
			return false;
	}
	
	return true;
}

void CanvasTabWidget::activate()
{
	auto canvasView = canvasViewAt(currentIndex());
	emit currentCanvasChanged(canvasView ? canvasView->controller() : 0);
	emit activated();
}

void CanvasTabWidget::onTabCloseRequested(int index)
{
	auto canvasView = canvasViewAt(index);
	if (canvasView)
		canvasView->controller()->closeCanvas();
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
			auto canvas = CanvasController::fromFile(url.toLocalFile(), _tabWidget->workspace());
			if (canvas)
				_tabWidget->insertCanvas(insertionIndexAt(event->pos()), canvas);
		}
		event->acceptProposedAction();
	}
}


} // namespace PaintField
