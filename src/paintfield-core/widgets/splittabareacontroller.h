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
	
	bool isInsertableFrom(DockTabWidget *other) override;
	void insertTab(int index, QWidget *widget, const QString &title) override;
	QObject *createNew() override;
	
private slots:
	
	void notifyTabChange();
	
private:
	
	void commonInit();
	
	SplitTabAreaController *_tabAreaController;
};

class SplitTabDefaultWidget : public QWidget
{
	Q_OBJECT
	
public:
	
	SplitTabDefaultWidget(SplitTabWidget *tabWidget, QWidget *parent);
	
signals:
	
	void clicked();
	
protected:
	
	void mousePressEvent(QMouseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	
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
	void onStackedWidgetClicked();
	
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
	
public:
	SplitTabAreaController(QWidget *baseWindow, QObject *parent = 0);
	
	QWidget *view() { return _rootSplit->splitter(); }
	
	void addTab(QWidget *tab, const QString &title);
	
	void registerTabWidget(SplitTabWidget *widget);
	
signals:
	
	void currentTabChanged(QWidget *tab);
	
public slots:
	
	void split(Qt::Orientation orientation);
	void closeCurrentSplit();
	
	void setCurrentTabWidget(SplitTabWidget *tabWidget);
	void setCurrentTab(QWidget *tab);
	
	void removeTab(QWidget *tab);
	
private slots:
	
	void onCurrentTabWidgetCurrentChanged(int index);
	void onBaseWindowFocusChanged(bool focused);
	
	void onTabWidgetAboutToBeDeleted(DockTabWidget *widget);
	
private:
	
	void setCurrentSplit(SplitAreaController *split);
	
	SplitTabWidget *tabWidgetForTab(QWidget *tab);
	SplitTabWidget *tabWidgetForSplit(SplitAreaController *split);
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
