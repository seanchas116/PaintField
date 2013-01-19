#include "canvastabwidget.h"

#include "canvassplitwidget.h"

namespace PaintField {

CanvasSplitDefaultWidget::CanvasSplitDefaultWidget(CanvasTabWidget *tabWidget, QWidget *parent) :
    QWidget(parent),
    _tabWidget(tabWidget)
{
	setAcceptDrops(true);
}

bool CanvasSplitDefaultWidget::dropDockTab(DockTabWidget *srcTabWidget, int srcIndex, const QPoint &pos)
{
	Q_UNUSED(pos)
	
	srcTabWidget->moveTab(srcIndex, _tabWidget, 0);
	
	return true;
}

bool CanvasSplitDefaultWidget::tabIsInsertable(DockTabWidget *src, int srcIndex)
{
	return _tabWidget->tabIsInsertable(src, srcIndex);
}

void CanvasSplitDefaultWidget::mousePressEvent(QMouseEvent *)
{
	emit activated();
}

CanvasSplitWidget::CanvasSplitWidget(CanvasTabWidget *tabWidget, QWidget *parent) :
    QStackedWidget(parent),
    _tabWidget(tabWidget),
    _defaultWidget(new CanvasSplitDefaultWidget(tabWidget, 0))
{
	_tabWidget->setAutoDeletionEnabled(false);
	
	insertWidget(IndexTabWidget, tabWidget);
	insertWidget(IndexDefaultWidget, _defaultWidget);
	
	setCurrentIndex(IndexDefaultWidget);
	
	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabWidgetCurrentChanged(int)));
	connect(tabWidget, SIGNAL(activated()), this, SIGNAL(activated()));
	connect(_defaultWidget, SIGNAL(activated()), this, SIGNAL(activated()));
}

bool CanvasSplitWidget::tryClose()
{
	return _tabWidget->tryClose();
}

void CanvasSplitWidget::onTabWidgetCurrentChanged(int index)
{
	if (index >= 0)
		setCurrentIndex(IndexTabWidget);
	else
		setCurrentIndex(IndexDefaultWidget);
}

} // namespace PaintField
