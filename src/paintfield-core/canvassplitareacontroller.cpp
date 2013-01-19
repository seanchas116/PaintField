#include <QSplitter>
#include "canvascontroller.h"
#include "canvastabwidget.h"
#include "canvassplitwidget.h"

#include "canvassplitareacontroller.h"

namespace PaintField {

CanvasSplitAreaController::CanvasSplitAreaController(WorkspaceView *workspaceView, QObject *parent) :
    QObject(parent),
    _workspaceView(workspaceView)
{
	auto split = createSplitWidget();
	
	_rootSplitter = new QSplitter;
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

void CanvasSplitAreaController::addCanvas(CanvasController *canvas)
{
	_currentSplit->tabWidget()->addTab(canvas->view(), canvas->document()->fileName());
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
	auto tabWidget = new CanvasTabWidget(_workspaceView, 0);
	auto splitWidget = new CanvasSplitWidget(tabWidget, 0);
	connect(splitWidget, SIGNAL(activated()), this, SLOT(onSplitActivated()));
	return splitWidget;
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

void CanvasSplitAreaController::addSplit(QWidget *existingSplit, QWidget *newSplit, Qt::Orientation orientation)
{
	auto splitter = splitterForWidget(existingSplit);
	Q_ASSERT(splitter);
	
	auto newSplitter = new QSplitter(orientation);
	replaceSplitterWidget(splitter, existingSplit, newSplitter);
	newSplitter->addWidget(existingSplit);
	newSplitter->addWidget(newSplit);
}

void CanvasSplitAreaController::removeSplit(CanvasSplitWidget *split)
{
	auto splitter = splitterForWidget(split);
	if (splitter == _rootSplitter && splitter->count() == 1)
		return;
	
	auto oppositeSplit = oppositeSplitterWidget(splitter, split);
	
	if (split == _currentSplit)
	{
		setCurrentSplit(oppositeSplit);
	}
	
	if (splitter != _rootSplitter)
	{
		auto parentSplitter = splitterForWidget(splitter);
		replaceSplitterWidget(parentSplitter, splitter, oppositeSplit);
	}
}

void CanvasSplitAreaController::setCurrentSplit(QWidget *splitOrSplitter)
{
	auto split = qobject_cast<CanvasSplitWidget *>(splitOrSplitter);
	if (split)
	{
		_currentSplit = split;
		return;
	}
	
	QSplitter *splitter = qobject_cast<QSplitter *>(splitOrSplitter);
	if (splitter && splitter->count())
	{
		setCurrentSplit(splitter->widget(0));
		return;
	}
	
	Q_ASSERT(0);
}

QSplitter *CanvasSplitAreaController::splitterForWidget(QWidget *widget)
{
	return qobject_cast<QSplitter *>(widget->parent());
}

} // namespace PaintField
