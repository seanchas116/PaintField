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
	setTabsClosable(true);
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
	connect(this, SIGNAL(tabClicked()), this, SIGNAL(activated()));
	connect(this, SIGNAL(tabMovedIn()), this, SIGNAL(activated()));
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
		emit activated();
}

bool CanvasTabWidget::tryClose()
{
	QList<CanvasController *> canvasList;
	
	for (int i = 0; i < count(); ++i)
	{
		auto canvasView = canvasViewAt(0);
		if (canvasView)
			canvasList << canvasView->controller();
	}
	
	for (auto canvas : canvasList)
	{
		if (!canvas->closeCanvas())
			return false;
	}
	
	return true;
}

void CanvasTabWidget::onActivated()
{
	auto canvasView = canvasViewAt(currentIndex());
	if (canvasView)
		emit currentCanvasChanged(canvasView->controller());
}

void CanvasTabWidget::onCurrentIndexChanged(int index)
{
	PAINTFIELD_DEBUG;
	if (index >= 0)
	{
		auto canvasView = canvasViewAt(index);
		if (canvasView)
		{
			emit currentCanvasChanged(canvasView->controller());
		}
		emit activated();
	}
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


} // namespace PaintField
