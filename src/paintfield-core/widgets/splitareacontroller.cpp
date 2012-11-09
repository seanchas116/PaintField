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
	
	_children[0].reset(node0);
	_children[1].reset(node1);
	
	_splitter->addWidget(node0->splitter());
	_splitter->addWidget(node1->splitter());
	
	node0->setParent(this);
	node1->setParent(this);
}

void SplitAreaController::clearChildNodes()
{
	_children[0].reset();
	_children[1].reset();
}

void SplitAreaController::promote(Index index)
{
	if (!isSplitted())
		return;
	
	if (_children[index]->isSplitted())
	{
		SplitAreaController *node0 = _children[index]->_children[0].take();
		SplitAreaController *node1 = _children[index]->_children[1].take();
		_children[0]->_splitter->setParent(0);
		_children[1]->_splitter->setParent(0);
		setChildNodes(node0, node1);
	}
	else
	{
		_widget = _children[index]->_widget;
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
