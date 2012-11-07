#include <QtGui>

#include "tabdocumentarea.h"

namespace PaintField
{

class TabDocumentAreaNode
{
public:
	
	enum Index
	{
		First = 0,
		Second = 1
	};
	
	TabDocumentAreaNode(TabDocumentArea *area, DockTabWidget *tabWidget);
	
	Index anotherIndex(Index index) { return index == First ? Second : First; }
	
	void insert(Index index, Qt::Orientation orientation);
	void close(Index index) { promote(anotherIndex(index)); }
	
	bool isSplitted() { return !_tabWidget; }
	
	QSplitter *splitter() { return _splitter.data(); }
	DockTabWidget *tabWidget() { return _tabWidget; }
	
	TabDocumentAreaNode *parent() { return _parent; }
	Index index() { return _index; }
	
	TabDocumentAreaNode *child(Index index) { return _children[index].data(); }
	
	QList<TabDocumentAreaNode *> children() { return isSplitted() ? QList<TabDocumentAreaNode *>({ _children[0].data(), _children[1].data() }) : QList<TabDocumentAreaNode *>(); }
	
	TabDocumentAreaNode *findNodeForTabWidget(DockTabWidget *widget);
	QList<DockTabWidget *> descendantTabWidgets();
	
private:
	
	void setNode(TabDocumentAreaNode *node0, TabDocumentAreaNode *node1);
	void promote(Index index);
	
	TabDocumentAreaNode *_parent = 0;
	Index _index = First;
	
	QScopedPointer<TabDocumentAreaNode> _children[2];
	ScopedQObjectPointer<QSplitter> _splitter;
	DockTabWidget *_tabWidget = 0;
	
	TabDocumentArea *_area = 0;
};

TabDocumentAreaNode::TabDocumentAreaNode(TabDocumentArea *area, DockTabWidget *tabWidget) :
	_splitter(new QSplitter),
	_tabWidget(tabWidget),
	_area(area)
{
	_splitter->addWidget(tabWidget);
	QObject::connect(tabWidget, SIGNAL(activated()), _area, SLOT(onSplitActivated()));
}

void TabDocumentAreaNode::insert(Index index, Qt::Orientation orientation)
{
	if (isSplitted())
		return;
	
	_splitter->setOrientation(orientation);
	
	auto node = new TabDocumentAreaNode(_area,  _tabWidget);
	auto newNode = new TabDocumentAreaNode(_area, _tabWidget->createAnother());
	
	if (index == First)
		setNode(newNode, node);
	else
		setNode(node, newNode);
		
	_tabWidget = 0;
}

void TabDocumentAreaNode::setNode(TabDocumentAreaNode *node0, TabDocumentAreaNode *node1)
{
	node0->_parent = this;
	node0->_index = First;
	
	node1->_parent = this;
	node1->_index = Second;
	
	_children[0].reset(node0);
	_children[1].reset(node1);
	
	_splitter->addWidget(node0->splitter());
	_splitter->addWidget(node1->splitter());
}

void TabDocumentAreaNode::promote(Index index)
{
	if (!isSplitted())
		return;
	
	if (_children[index]->isSplitted())
	{
		TabDocumentAreaNode *node0 = _children[index]->_children[0].take();
		TabDocumentAreaNode *node1 = _children[index]->_children[1].take();
		_children[0]->_splitter->setParent(0);
		_children[1]->_splitter->setParent(0);
		setNode(node0, node1);
	}
	else
	{
		_tabWidget = _children[index]->_tabWidget;
		_splitter->addWidget(_tabWidget);
		_children[0].reset();
		_children[1].reset();
		_tabWidget->activate();
	}
}

TabDocumentAreaNode *TabDocumentAreaNode::findNodeForTabWidget(DockTabWidget *widget)
{
	if (tabWidget() == widget)
		return this;
	
	if (isSplitted())
	{
		for (TabDocumentAreaNode *child : children())
		{
			TabDocumentAreaNode *found = child->findNodeForTabWidget(widget);
			if (found)
				return found;
		}
	}
	
	return 0;
}

QList<DockTabWidget *> TabDocumentAreaNode::descendantTabWidgets()
{
	QList<DockTabWidget *> list;
	
	if (tabWidget())
		list << tabWidget();
	
	if (isSplitted())
	{
		for (TabDocumentAreaNode *child : children())
			list << child->descendantTabWidgets();
	}
	
	return list;
}

TabDocumentArea::TabDocumentArea(DockTabWidget *tabWidget, QWidget *parent) :
	QWidget(parent),
	_rootNode(new TabDocumentAreaNode(this, tabWidget)),
	_currentNode(_rootNode.data()),
	_currentTabWidget(tabWidget)
{
	auto layout = new QVBoxLayout;
	layout->addWidget(_rootNode->splitter());
	layout->setContentsMargins(0,0,0,0);
	setLayout(layout);
}

TabDocumentArea::~TabDocumentArea()
{
}

void TabDocumentArea::addTab(const QString &title, QWidget *tab)
{
	_currentTabWidget->addTab(tab, title);
}

void TabDocumentArea::takeTab(QWidget *tab)
{
	auto tabWidget = tabWidgetForTab(tab);
	
	int index =tabWidget->indexOf(tab);
	
	tabWidget->removeTab(index);
}

void TabDocumentArea::setCurrentTab(QWidget *tab)
{
	auto tabWidget = tabWidgetForTab(tab);
	if (tabWidget)
		tabWidget->setCurrentWidget(tab);
}

void TabDocumentArea::splitCurrent(Qt::Orientation orientation)
{
	_currentNode->insert(TabDocumentAreaNode::Second, orientation);
}

void TabDocumentArea::closeCurrent()
{
	_currentNode->parent()->close(_currentNode->index());
}

void TabDocumentArea::onSplitActivated()
{
	PAINTFIELD_PRINT_DEBUG("split activated");
	
	DockTabWidget *tabWidget = qobject_cast<DockTabWidget *>(sender());
	if (tabWidget)
	{
		auto node = _rootNode->findNodeForTabWidget(tabWidget);
		if (node)
			setCurrentNode(node);
	}
}

void TabDocumentArea::updateCurrentTab()
{
	emit currentTabChanged(_currentTabWidget->currentWidget());
}

DockTabWidget *TabDocumentArea::tabWidgetForTab(QWidget *tab)
{
	for (DockTabWidget *tabWidget : _rootNode->descendantTabWidgets())
	{
		if (tabWidget->contains(tab))
			return tabWidget;
	}
	return 0;
}

void TabDocumentArea::setCurrentNode(TabDocumentAreaNode *node)
{
	_currentNode = node;
	Q_ASSERT(_currentNode);
	_currentTabWidget = node->tabWidget();
	Q_ASSERT(_currentTabWidget);
	
	emit currentTabChanged(_currentTabWidget->currentWidget());
}



}
