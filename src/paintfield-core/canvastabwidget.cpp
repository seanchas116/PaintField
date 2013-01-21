#include <QUrl>
#include "workspacecontroller.h"
#include "appcontroller.h"
#include "workspacemanager.h"
#include "debug.h"

#include "canvastabwidget.h"

namespace PaintField {

struct CanvasTabWidget::Data
{
	WorkspaceController *workspace = 0;
	bool floating = false;
};

CanvasTabWidget::CanvasTabWidget(WorkspaceView *workspaceView, QWidget *parent) :
    DockTabWidget(parent),
    d(new Data)
{
	d->workspace = workspaceView->controller();
	commonInit();
}

CanvasTabWidget::CanvasTabWidget(CanvasTabWidget *other, QWidget *parent) :
    DockTabWidget(other, parent),
    d(new Data)
{
	d->workspace = other->workspace();
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
	setAutoDeletionEnabled(true);
	
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(activate()));
	connect(this, SIGNAL(tabClicked()), this, SIGNAL(activated()));
	connect(this, SIGNAL(tabMovedIn(QWidget*)), this, SIGNAL(activated()));
	
	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
	
	connect(d->workspace, SIGNAL(currentCanvasChanged(CanvasController*)), this, SLOT(onCurrentCanvasChanged(CanvasController*)));
	connect(this, SIGNAL(currentCanvasChanged(CanvasController*)), d->workspace, SLOT(setCurrentCanvas(CanvasController*)));
	
	connect(appController()->workspaceManager(), SIGNAL(currentWorkspaceChanged(WorkspaceController*)), this, SLOT(onCurrentWorkspaceChanged(WorkspaceController*)));
	//onCurrentWorkspaceChanged(appController()->workspaceManager()->currentWorkspace());
	
	if (parent() == 0)
		setFloating(true);
}

bool CanvasTabWidget::tabIsInsertable(DockTabWidget *other, int index)
{
	Q_UNUSED(index)
	
	CanvasTabWidget *tabWidget = qobject_cast<CanvasTabWidget *>(other);
	return tabWidget && tabWidget->d->workspace == d->workspace;
}

bool CanvasTabWidget::isFloating() const
{
	return d->floating;
}

void CanvasTabWidget::setFloating(bool x)
{
	d->floating = x;
	
	if (x)
	{
		setParent(d->workspace->view());
		setWindowFlags(Qt::Tool);
	}
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
	workspace()->addCanvas(canvas);
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

void CanvasTabWidget::onCurrentCanvasChanged(CanvasController *canvas)
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

void CanvasTabWidget::onCurrentWorkspaceChanged(WorkspaceController *workspace)
{
	if (d->floating)
		setVisible(workspace == d->workspace);
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
			auto canvas = CanvasController::fromFile(url.toLocalFile());
			if (canvas)
				_tabWidget->insertCanvas(insertionIndexAt(event->pos()), canvas);
		}
		event->acceptProposedAction();
	}
}


} // namespace PaintField
