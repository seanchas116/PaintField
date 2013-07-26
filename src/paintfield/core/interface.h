#pragma once

#include <QObject>

namespace PaintField
{

class ReproductiveInterface
{
public:
	virtual ~ReproductiveInterface() {}
	
	virtual QObject *createNew() = 0;
	
	template <class T>
	T *createNewAs()
	{
		auto obj = createNew();
		auto as = qobject_cast<T *>(obj);
		Q_ASSERT(as);
		return as;
	}
};

class DockTabWidget;
class DockTabBar;

class DockTabDroppableInterface
{
public:
	
	virtual ~DockTabDroppableInterface() {}
	virtual bool dropDockTab(DockTabWidget *srcTabWidget, int srcIndex, const QPoint &pos) = 0;
	virtual bool tabIsInsertable(DockTabWidget *src, int srcIndex) = 0;
};

}

Q_DECLARE_INTERFACE(PaintField::ReproductiveInterface, "PaintField.ReproductiveInterface")
Q_DECLARE_INTERFACE(PaintField::DockTabDroppableInterface, "PaintField.DockTabDroppableInterface")
