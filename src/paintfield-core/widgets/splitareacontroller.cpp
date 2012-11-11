#include <QSplitter>

#include "splitareacontroller.h"

namespace PaintField
{

SplitAreaController::SplitAreaController(QWidget *widget, QObject *parent) :
	QObject(parent),
	_splitter(new QSplitter),
	_widget(widget)
{
	Q_ASSERT(qobject_cast<ReproductiveInterface *>(widget));
	_splitter->addWidget(widget);
}

void SplitAreaController::insert(Index index, Qt::Orientation orientation)
{
	if (isSplitted())
		return;
	
	_splitter->setOrientation(orientation);
	
	auto node = new SplitAreaController(_widget, this);
	auto newNode = new SplitAreaController(qobject_cast<ReproductiveInterface *>(_widget)->createNewAs<QWidget>(), this);
	
	if (index == First)
		setChildNodes(newNode, node);
	else
		setChildNodes(node, newNode);
		
	_widget = 0;
}

void SplitAreaController::setChildNodes(SplitAreaController *node0, SplitAreaController *node1)
{
	node0->_parent = this;
	node0->_index = First;
	
	node1->_parent = this;
	node1->_index = Second;
	
	childSplitRef(First).reset(node0);
	childSplitRef(Second).reset(node1);
	
	_splitter->addWidget(node0->splitter());
	_splitter->addWidget(node1->splitter());
	
	node0->setParent(this);
	node1->setParent(this);
}

void SplitAreaController::clearChildNodes()
{
	childSplitRef(First).reset();
	childSplitRef(Second).reset();
}

void SplitAreaController::promote(Index index)
{
	if (!isSplitted())
		return;
	
	if (childSplit(index)->isSplitted())
	{
		SplitAreaController *node0 = childSplit(index)->childSplitRef(First).take();
		SplitAreaController *node1 = childSplit(index)->childSplitRef(Second).take();
		childSplit(First)->_splitter->setParent(0);
		childSplit(Second)->_splitter->setParent(0);
		setChildNodes(node0, node1);
	}
	else
	{
		_widget = childSplit(index)->_widget;
		_splitter->addWidget(_widget);
		clearChildNodes();
	}
}

SplitAreaController *SplitAreaController::firstNonSplittedDescendant()
{
	SplitAreaController *split = this;
	
	while (split->isSplitted())
		split = split->childSplit(First);
	
	return split;
}

SplitAreaController *SplitAreaController::findSplitForWidget(QWidget *widget)
{
	return findSplit([widget](SplitAreaController *split)->bool
	{
		return split->widget() == widget;
	});
}

QWidgetList SplitAreaController::descendantWidgets()
{
	QWidgetList list;
	
	if (widget())
		list << widget();
	
	if (isSplitted())
	{
		for (SplitAreaController *child : childSplits())
			list += child->descendantWidgets();
	}
	
	return list;
}

}
