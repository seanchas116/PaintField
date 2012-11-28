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
	
	explicit DockTabWidget(QWidget *parent);
	
	/**
	 * Creates a DockTabWidget copying other's DockTabWidget specific settings.
	 * @param other
	 * @param parent
	 */
	DockTabWidget(DockTabWidget *other, QWidget *parent);
	
	void setAutoDeletionEnabled(bool x) { _autoDeletionEnabled = x; }
	bool isAutoDeletionEnabled() const { return _autoDeletionEnabled; }
	
	bool contains(QWidget *widget) { return indexOf(widget) >= 0; }
	
	void moveTab(int index, DockTabWidget *dst, int dstIndex) { moveTab(this, index, dst, dstIndex); }
	static void moveTab(DockTabWidget *source, int sourceIndex, DockTabWidget *dest, int destIndex);
	static bool eventIsTabDrag(QDragEnterEvent *event);
	
	virtual bool tabIsInsertable(DockTabWidget *other, int index) { Q_UNUSED(other) Q_UNUSED(index) return true; }
	virtual void insertTab(int index, QWidget *widget, const QString &title) { QTabWidget::insertTab(index, widget, title); }
	
	QObject *createNew() override;
	DockTabWidget *createNewTabWidget() { return createNewAs<DockTabWidget>(); }
	
signals:
	
	void tabClicked();
	void focusIn();
	void focusOut();
	void willBeAutomaticallyDeleted(DockTabWidget *widget);
	
public slots:
	
	void deleteIfEmpty();
	
protected:
	
	void focusInEvent(QFocusEvent *) override;
	void focusOutEvent(QFocusEvent *) override;
	void closeEvent(QCloseEvent *event) override;
	
private slots:
	
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
	
signals:
	
	void clicked();
	
protected:
	
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	
private:
	
	void dragDropTab(int index, const QPoint &globalPos, const QPoint &dragStartOffset);
	int insertionIndexAt(const QPoint &pos);
	
	DockTabWidget *_tabWidget = 0;
	bool _isStartingDrag = false;
	QPoint _dragStartPos;
	int _dragIndex;
};

}

#endif // DRAGGABLETABWIDGET_H
