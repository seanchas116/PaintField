#ifndef DRAGGABLETABWIDGET_H
#define DRAGGABLETABWIDGET_H

#include <QTabBar>
#include <QTabWidget>
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
	
	explicit DockTabWidget(QWidget *parent = 0);
	
	void setAutoDeletionEnabled(bool x) { _autoDeletionEnabled = x; }
	bool isAutoDeletionEnabled() const { return _autoDeletionEnabled; }
	
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
	
	void tabAboutToMoveOut(QWidget *tab);
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
	
	bool _autoDeletionEnabled = false;
};

class DockTabBar : public QTabBar, public DockTabDroppableInterface
{
	Q_OBJECT
	Q_INTERFACES(PaintField::DockTabDroppableInterface)
	
public:
	
	DockTabBar(DockTabWidget *tabWidget, QWidget *parent = 0);
	
	DockTabWidget *tabWidget() { return _tabWidget; }
	
	bool tabIsInsertable(DockTabWidget *src, int srcIndex) override
	{
		return _tabWidget->tabIsInsertable(src, srcIndex);
	}

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
	
	DockTabWidget *_tabWidget = 0;
	bool _isStartingDrag = false;
	QPoint _dragStartPos;
	int _dragIndex;
};

}

#endif // DRAGGABLETABWIDGET_H
