#ifndef SPLITAREACONTROLLER_H
#define SPLITAREACONTROLLER_H

#include <QSplitter>
#include "../smartpointer.h"

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
	
	/**
	 * Constructs a controller of a split area.
	 * "this" will take ownership of the widget.
	 * @param widget A content widget
	 * @param parent
	 */
	SplitAreaController(QWidget *widget, QObject *parent);
	
	Index anotherIndex(Index index) { return index == First ? Second : First; }
	
	void insert(Index index, Qt::Orientation orientation);
	
	/**
	 * Closes a child split area.
	 * It and its descendant split areas will be deleted.
	 * @param index
	 */
	void close(Index index) { promote(anotherIndex(index)); }
	
	/**
	 * @return Whether "this" is splitted (there are child split areas)
	 */
	bool isSplitted() { return !_widget; }
	
	QWidget *view() { return splitter(); }
	
	/**
	 * @return The splitter. Returns a valid splitter no matter "this" is splitted.
	 */
	QSplitter *splitter() { return _splitter.data(); }
	
	/**
	 * @return The content widget. Returns 0 if "this" is splitted.
	 */
	QWidget *widget() { return _widget; }
	
	/**
	 * @return Parent split area controller or 0 (no parent).
	 */
	SplitAreaController *parentSplit() { return _parent; }
	
	Index index() { return _index; }
	
	SplitAreaController *childSplit(Index index) { return childSplitRef(index).data(); }
	
	QList<SplitAreaController *> childSplits();
	
	/**
	 * @return The first widget which is not splitted.
	 */
	SplitAreaController *firstNonSplittedDescendant();
	
	/**
	 * @param widget
	 * @return The first widget which contains the widget
	 */
	SplitAreaController *findSplitForWidget(QWidget *widget);
	
	/**
	 * @param pred Predicate function
	 * @return The first widget for which pred(widget) == true
	 */
	template <typename Predicate> SplitAreaController *findSplit(Predicate pred);
	
	/**
	 * @return The widgets which "this" and its descendants contain
	 */
	QWidgetList descendantWidgets();
	
private:
	
	void setChildNodes(SplitAreaController *node0, SplitAreaController *node1);
	void clearChildNodes();
	void promote(Index index);
	
	ScopedQObjectPointer<SplitAreaController> &childSplitRef(Index index)
	{
		Q_ASSERT(0 <= index && index < 2);
		return _children[index];
	}
	
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
