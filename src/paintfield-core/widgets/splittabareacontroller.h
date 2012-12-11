#ifndef SPLITTABAREACONTROLLER_H
#define SPLITTABAREACONTROLLER_H

#include <QObject>
#include <QPointer>
#include <QStackedWidget>
#include "../util.h"
#include "splitareacontroller.h"
#include "floatingdocktabwidget.h"

namespace PaintField
{

class SplitTabAreaController;

class SplitTabWidget : public FloatingDockTabWidget
{
	Q_OBJECT
	Q_INTERFACES(PaintField::ReproductiveInterface)
	
	typedef DockTabWidget super;
	
public:
	
	SplitTabWidget(SplitTabAreaController *tabAreaController, QWidget *baseWindow, QWidget *parent);
	SplitTabWidget(SplitTabWidget *other, QWidget *parent);
	
	bool tabIsInsertable(DockTabWidget *other, int index) override;
	void insertTab(int index, QWidget *widget, const QString &title) override;
	QObject *createNew() override;
	
private slots:
	
	void notifyTabChange();
	
private:
	
	void commonInit();
	
	SplitTabAreaController *_tabAreaController;
};

class SplitTabDefaultWidget : public QWidget, public DockTabDroppableInterface
{
	Q_OBJECT
	Q_INTERFACES(PaintField::DockTabDroppableInterface)
	
public:
	
	SplitTabDefaultWidget(SplitTabWidget *tabWidget, QWidget *parent);
	
	bool dropDockTab(DockTabWidget *srcTabWidget, int srcIndex, const QPoint &pos) override;
	bool tabIsInsertable(DockTabWidget *src, int srcIndex) override;
	
signals:
	
	void clicked();
	
protected:
	
	void mousePressEvent(QMouseEvent *event);
	
private:
	
	SplitTabWidget *_tabWidget;
};

class SplitTabStackedWidget : public QStackedWidget, public ReproductiveInterface
{
	Q_OBJECT
	Q_INTERFACES(PaintField::ReproductiveInterface)
	
public:
	
	SplitTabStackedWidget(SplitTabAreaController *tabAreaController, SplitTabWidget *tabWidget, QWidget *parent);
	
	SplitTabWidget *tabWidget() { return _tabWidget; }
	SplitTabDefaultWidget *defaultWidget() { return _defaultWidget; }
	
	QObject *createNew() override;
	
signals:
	
private slots:
	
	void onTabWidgetCurrentChanged(int index);
	
private:
	
	enum Index
	{
		IndexTabWidget = 0,
		IndexDefaultWidget = 1
	};
	
	SplitTabAreaController *_tabAreaController;
	SplitTabWidget *_tabWidget;
	SplitTabDefaultWidget *_defaultWidget;
};

class SplitTabAreaController : public QObject
{
	Q_OBJECT
	friend class SplitTabWidget;
	friend class SplitTabStackedWidget;
	
public:
	SplitTabAreaController(QWidget *baseWindow, QObject *parent = 0);
	
	/**
	 * @return The view
	 */
	QWidget *view() { return _rootSplit->splitter(); }
	
	/**
	 * Adds a tab.
	 * @param tab
	 * @param title
	 */
	void addTab(QWidget *tab, const QString &title);
	
	/**
	 * @param tab
	 * @return The tab widget which contains tab
	 */
	FloatingDockTabWidget *tabWidgetForTab(QWidget *tab);
	
signals:
	
	/**
	 * The current tab is changed.
	 * @param tab
	 */
	void currentTabChanged(QWidget *tab);
	
	/**
	 * A tab is requested to be closed.
	 * @param tab
	 */
	void tabCloseRequested(QWidget *tab);
	
public slots:
	
	void splitCurrentSplit(Qt::Orientation orientation);
	void closeCurrentSplit();
	void setCurrentTab(QWidget *tab);
	void removeTab(QWidget *tab);
	
private slots:
	
	void setCurrentTabWidget(SplitTabWidget *tabWidget);
	void onCurrentTabWidgetCurrentChanged(int index);
	void onTabWidgetCloseRequested(int index);
	
	void onTabWidgetAboutToBeDeleted(DockTabWidget *widget);
	
private:
	
	void registerTabWidget(SplitTabWidget *widget);
	
	void setCurrentSplit(SplitAreaController *splitCurrentSplit);
	
	SplitTabWidget *tabWidgetForSplit(SplitAreaController *splitCurrentSplit);
	SplitTabWidget *tabWidgetForCurrentSplit() { return tabWidgetForSplit(_currentSplit); }
	SplitAreaController *splitForWidget(QWidget *widget);
	SplitAreaController *splitForTabWidget(SplitTabWidget *tabWidget);
	
	SplitAreaController *_rootSplit = 0;
	SplitAreaController *_currentSplit = 0;
	
	QList<SplitTabWidget *> _tabWidgets;
	SplitTabWidget *_currentTabWidget = 0;
	QWidget *_currentTab = 0;
};

}

#endif // SPLITTABAREACONTROLLER_H
