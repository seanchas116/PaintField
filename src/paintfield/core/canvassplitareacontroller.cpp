#include "canvas.h"
#include "canvasview.h"
#include "canvastabwidget.h"
#include "canvassplitwidget.h"
#include "widgets/memorizablesplitter.h"

#include "canvassplitareacontroller.h"

namespace PaintField {

CanvasSplitAreaController::CanvasSplitAreaController(Workspace *workspace, QObject *parent) :
    QObject(parent),
    _workspace(workspace)
{
	auto split = createSplitWidget();
	
	_rootSplitter = new MemorizableSplitter;
	_rootSplitter->addWidget(split);
	
	_currentSplit = split;
}

void CanvasSplitAreaController::splitCurrent(Qt::Orientation orientation)
{
	addSplit(_currentSplit, createSplitWidget(), orientation);
}

void CanvasSplitAreaController::closeCurrent()
{
	removeSplit(_currentSplit);
}

void CanvasSplitAreaController::addCanvas(Canvas *canvas)
{
	new CanvasView(canvas);
	_currentSplit->tabWidget()->addCanvas(canvas);
}

QWidget *CanvasSplitAreaController::view()
{
	return _rootSplitter;
}

void CanvasSplitAreaController::onSplitActivated()
{
	CanvasSplitWidget *split = qobject_cast<CanvasSplitWidget *>(sender());
	Q_ASSERT(split);
	_currentSplit = split;
}

CanvasSplitWidget *CanvasSplitAreaController::createSplitWidget()
{
	auto tabWidget = new CanvasTabWidget(_workspace, 0);
	auto splitWidget = new CanvasSplitWidget(tabWidget, 0);
	connect(splitWidget, SIGNAL(activated()), this, SLOT(onSplitActivated()));
	return splitWidget;
}

static QWidgetList splitterWidgetList(QSplitter *splitter)
{
	QWidgetList list;
	
	for (int i = 0; i < splitter->count(); ++i)
		list << splitter->widget(i);
	
	return list;
}

static void replaceSplitterWidget(QSplitter *splitter, QWidget *before, QWidget *after)
{
	int index = splitter->indexOf(before);
	if (index >= 0)
	{
		before->setParent(0);
		splitter->insertWidget(index, after);
	}
}

static QWidget *oppositeSplitterWidget(QSplitter *splitter, QWidget *widget)
{
	Q_ASSERT(splitter->count() == 2);
	int index = splitter->indexOf(widget);
	Q_ASSERT(index == 0 || index == 1);
	
	return splitter->widget((index == 0) ? 1 : 0);
}

void CanvasSplitAreaController::addSplit(CanvasSplitWidget *existingSplit, CanvasSplitWidget *newSplit, Qt::Orientation orientation)
{
	auto splitter = splitterForWidget(existingSplit);
	Q_ASSERT(splitter);
	
	existingSplit->tabWidget()->memorizeTransforms();
	splitter->memorizeSizes();
	
	auto newSplitter = new MemorizableSplitter(orientation);
	replaceSplitterWidget(splitter, existingSplit, newSplitter);
	newSplitter->addWidget(existingSplit);
	newSplitter->addWidget(newSplit);
	
	newSplitter->setSizes({1000, 1000});
	
	existingSplit->tabWidget()->restoreTransforms();
	splitter->restoreSizes();
}

void CanvasSplitAreaController::removeSplit(CanvasSplitWidget *split)
{
	auto splitter = splitterForWidget(split);
	
	Q_ASSERT(splitter);
	
	if (splitter == _rootSplitter)
		return;
	
	if (!split->tryClose())
		return;
	
	CanvasSplitWidget *oppositeSplit = qobject_cast<CanvasSplitWidget *>(oppositeSplitterWidget(splitter, split));
	Q_ASSERT(oppositeSplit);
	
	if (split == _currentSplit)
		setCurrentSplit(oppositeSplit);
	
	auto parentSplitter = splitterForWidget(splitter);
	
	oppositeSplit->tabWidget()->memorizeTransforms();
	parentSplitter->memorizeSizes();
	
	replaceSplitterWidget(parentSplitter, splitter, oppositeSplit);
	
	oppositeSplit->tabWidget()->restoreTransforms();
	parentSplitter->restoreSizes();
	
	split->deleteLater();
	splitter->deleteLater();
}

void CanvasSplitAreaController::setCurrentSplit(QWidget *splitOrSplitter)
{
	auto split = qobject_cast<CanvasSplitWidget *>(splitOrSplitter);
	if (split)
	{
		_currentSplit = split;
		return;
	}
	
	MemorizableSplitter *splitter = qobject_cast<MemorizableSplitter *>(splitOrSplitter);
	if (splitter && splitter->count())
	{
		setCurrentSplit(splitter->widget(0));
		return;
	}
	
	Q_ASSERT(0);
}

MemorizableSplitter *CanvasSplitAreaController::splitterForWidget(QWidget *widget)
{
	return qobject_cast<MemorizableSplitter *>(widget->parent());
}

} // namespace PaintField
