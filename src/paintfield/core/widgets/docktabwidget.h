#pragma once

#include <QTabBar>
#include <QTabWidget>
#include "../global.h"
#include "../interface.h"

namespace PaintField
{

class DockTabBar;

class DockTabWidget : public QTabWidget, public ReproductiveInterface
{
	Q_OBJECT
	Q_INTERFACES(PaintField::ReproductiveInterface)
	
	friend class DockTabBar;
	
public:
	
	explicit DockTabWidget(QWidget *baseWindow, QWidget *parent);
	~DockTabWidget();
	
	void makeFloating();
	
	bool isFloating() const;
	
	void setAutoDeletionEnabled(bool x);
	bool isAutoDeletionEnabled() const;
	
	bool contains(QWidget *widget) { return indexOf(widget) >= 0; }
	
	QWidgetList tabs();
	
	bool moveTab(int index, DockTabWidget *dst, int dstIndex) { return moveTab(this, index, dst, dstIndex); }
	static bool moveTab(DockTabWidget *source, int sourceIndex, DockTabWidget *dest, int destIndex);
	static bool eventIsTabDrag(QDragEnterEvent *event);
	
	virtual bool tabIsInsertable(DockTabWidget *src, int srcIndex) { Q_UNUSED(src) Q_UNUSED(srcIndex) return true; }
	virtual void insertTab(int index, QWidget *widget, const QString &title) { QTabWidget::insertTab(index, widget, title); }
	
	QObject *createNew() override;
	DockTabWidget *createNewTabWidget() { return createNewAs<DockTabWidget>(); }
	
	void requestCloseAllTabs();
	
signals:
	
	void tabAboutToBeMovedOut(QWidget *tab);
	void tabMovedIn(QWidget *tab);
	
	void tabClicked();
	void closeAllTabsRequested();
	
	void willBeAutomaticallyDeleted(DockTabWidget*);
	
public slots:
	
protected:
	
	void mousePressEvent(QMouseEvent *event) override;
	void closeEvent(QCloseEvent *event) override;
	
private slots:
	
	void onCurrentChanged(int index);
	
private:
	
	struct Data;
	Data *d;
};

class DockTabBar : public QTabBar, public DockTabDroppableInterface
{
	Q_OBJECT
	Q_INTERFACES(PaintField::DockTabDroppableInterface)
	
public:
	
	DockTabBar(DockTabWidget *tabWidget, QWidget *parent);
	~DockTabBar();
	
	DockTabWidget *tabWidget();
	
	bool tabIsInsertable(DockTabWidget *src, int srcIndex) override;
	bool dropDockTab(DockTabWidget *srcTabWidget, int srcIndex, const QPoint &pos) override;
	
	int insertionIndexAt(const QPoint &pos);
	
signals:
	
	void clicked();
	
protected:
	
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	
private:
	
	void dragDropTab(int index, const QPoint &globalPos, const QPoint &dragStartOffset);
	
	struct Data;
	Data *d;
};

}

