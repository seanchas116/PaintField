#ifndef MOTHERWIDGET_H
#define MOTHERWIDGET_H

#include <QWidget>
#include <QHash>

#include "../global.h"
#include "docktabwidget.h"

class QSplitter;
typedef QList<QSplitter *> QSplitterList;

namespace PaintField
{

class DockTabMotherWidget : public QWidget, public DockTabDroppableInterface
{
	Q_OBJECT
	Q_INTERFACES(PaintField::DockTabDroppableInterface)
	
public:
	
	explicit DockTabMotherWidget(QWidget *parent = 0);
	
	constexpr static int insertDistance() { return 20; }
	
	enum Direction
	{
		NoDirection,
		Left,
		Right,
		Top,
		Bottom
	};
	
	enum InsertionDirection
	{
		NextSplitter,
		PreviousSplitter,
		Next,
		Previous
	};
	
	struct TabWidgetArea
	{
		TabWidgetArea() : splitterIndex(-1) {}
		TabWidgetArea(Direction dir, int splitterIndex, int tabWidgetIndex) :
			dir(dir), splitterIndex(splitterIndex), tabWidgetIndex(tabWidgetIndex) {}
		
		bool isValid() const { return splitterIndex >= 0; }
		
		Direction dir;
		int splitterIndex, tabWidgetIndex;
	};
	
	bool insertTabWidget(DockTabWidget *tabWidget, const TabWidgetArea &area);
	
	void addTabWidget(DockTabWidget *tabWidget, Direction dir, int splitterIndex);
	
	int tabWidgetCount(Direction dir, int splitterIndex);
	int splitterCount(Direction dir);
	
	void setCentralWidget(QWidget *widget);
	
	void setSizesState(const QVariantMap &data);
	QVariantMap sizesState();
	
	void setTabIndexState(const QVariantMap &data);
	QVariantMap tabIndexState();
	
	QVariantMap tabObjectNameState();
	
	bool dropDockTab(DockTabWidget *srcTabWidget, int srcIndex, const QPoint &pos) override;
	bool tabIsInsertable(DockTabWidget *src, int srcIndex) override { Q_UNUSED(src) Q_UNUSED(srcIndex) return true; }
	
protected:
	
	QSplitter *createSplitter(Qt::Orientation orientation);
	
private slots:
	
	void onTabWidgetWillBeDeleted(DockTabWidget *widget);
	
private:
	
	template <typename TUnaryOperator>
	QVariantMap packDataForEachTabWidget(TUnaryOperator op);
	
	static QString stringFromDirection(Direction dir);
	
	TabWidgetArea dropArea(const QPoint &pos);
	TabWidgetArea dropAreaAt(const QPoint &pos, Direction dir);
	bool getInsertionDirection(const QPoint &pos, QWidget *widget, Direction dockDir, InsertionDirection &insertDir);
	
	QRect splittersRect(Direction dir);
	
	QHash<Direction, QSplitterList> _splitterLists;
	
	QWidget *_centralWidget = 0;
	QSplitter *_mainHorizontalSplitter, *_mainVerticalSplitter;
};

}

#endif // MOTHERWIDGET_H
