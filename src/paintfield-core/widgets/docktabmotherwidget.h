#ifndef MOTHERWIDGET_H
#define MOTHERWIDGET_H

#include <QWidget>
#include <QHash>

#include "docktabwidget.h"

class QSplitter;
typedef QList<QSplitter *> QSplitterList;

namespace PaintField
{

class DockTabMotherWidget : public QWidget
{
	Q_OBJECT
	
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
	
protected:
	
	QSplitter *createSplitter(Qt::Orientation orientation);
	
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	
private slots:
	
	void onTabWidgetWillBeDeleted(DockTabWidget *widget);
	
private:
	
	bool dropTab(DockTabWidget *tabWidget, int index, const QPoint &pos);
	TabWidgetArea dropArea(const QPoint &pos);
	TabWidgetArea dropAreaAt(const QPoint &pos, Direction dir);
	bool getInsertionDirection(const QPoint &pos, QWidget *widget, Direction dockDir, InsertionDirection &insertDir);
	
	QRect splittersRect(Direction dir);
	
	QHash<Direction, QSplitterList> _splitterLists;
	
	QWidget *_centralWidget = 0;
	QSplitter *_mainHorizontalSplitter, *_mainVerticalSplitter;
};

}

/*
class DockTabMotherWidget : public QWidget
{
	Q_OBJECT
public:
	
	explicit DockTabMotherWidget(QWidget *parent = 0);
	
protected:
	
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	
signals:
	
public slots:
	
private:
	
	enum
	{
		InsertDistance = 20,
		DirectionCount = 2
	};
	
	enum TabHandling
	{
		NoHandling,
		TabAppend,
		TabPrepend,
		NewColumn
	};
	
	enum Direction
	{
		Left = 0,
		Right = 1,
		Top = 2,
		Bottom = 3,
		NoDirection = -1
	};
	
	bool dropTab(DockTabWidget *tabWidget, int index, const QPoint &pos);
	
	void getTabHandling(const QPoint &dropPos, TabHandling &handling, Direction &columnDirection, int &index);
	Direction insertionDirection(QSplitter *splitter, const QPoint &pos);
	
private slots:
	
	void onTabWidgetWillBeDeleted(DockTabWidget *widget);
	
private:
	
	QList<QSplitter *> _columnSplitterLists[DirectionCount];
	
	QWidget *_centralWidget = 0;
	QSplitter *_mainSplitter = 0;
};*/

#endif // MOTHERWIDGET_H
