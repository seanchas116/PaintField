#ifndef SPLITAREACONTROLLER_H
#define SPLITAREACONTROLLER_H

#include <QSplitter>
#include "../util.h"

namespace PaintField
{

class SplitAreaController : public QObject
{
	Q_OBJECT
public:
	
	enum Index
	{
		First = 0,
		Second = 1
	};
	
	SplitAreaController(QWidget *widget, QObject *parentSplit);
	
	Index anotherIndex(Index index) { return index == First ? Second : First; }
	
	void insert(Index index, Qt::Orientation orientation);
	void close(Index index) { promote(anotherIndex(index)); }
	
	bool isSplitted() { return !_widget; }
	
	QWidget *view() { return _splitter.data(); }
	
	QSplitter *splitter() { return _splitter.data(); }
	QWidget *widget() { return _widget; }
	
	SplitAreaController *parentSplit() { return _parent; }
	Index index() { return _index; }
	
	SplitAreaController *childSplit(Index index) { return _children[index].data(); }
	
	QList<SplitAreaController *> childSplits() { return isSplitted() ? QList<SplitAreaController *>({ _children[0].data(), _children[1].data() }) : QList<SplitAreaController *>(); }
	
	SplitAreaController *firstNonSplittedDescendant();
	
	SplitAreaController *findSplitForWidget(QWidget *widget);
	
	template <typename Predicate> SplitAreaController *findSplit(Predicate pred);
	
	QWidgetList descendantWidgets();
	
private:
	
	void setChildNodes(SplitAreaController *node0, SplitAreaController *node1);
	void clearChildNodes();
	void promote(Index index);
	
	SplitAreaController *_parent = 0;
	Index _index = First;
	
	ScopedQObjectPointer<SplitAreaController> _children[2];
	ScopedQObjectPointer<QSplitter> _splitter;
	QWidget *_widget = 0;
};

template <typename Predicate>
SplitAreaController *SplitAreaController::findSplit(Predicate pred)
{
	if (pred(this))
		return this;
	
	if (isSplitted())
	{
		for (SplitAreaController *child : childSplits())
		{
			SplitAreaController *found = child->findSplit(pred);
			if (found)
				return found;
		}
	}
	
	return 0;
}

}

#endif // SPLITAREACONTROLLER_H
