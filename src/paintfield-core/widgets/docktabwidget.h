#ifndef DRAGGABLETABWIDGET_H
#define DRAGGABLETABWIDGET_H

#include <QTabBar>
#include <QTabWidget>

namespace PaintField
{

class DockTabBar;

class DockTabWidget : public QTabWidget
{
	Q_OBJECT
	friend class DockTabBar;
public:
	
	explicit DockTabWidget(QWidget *parent = 0);
	
	/**
	 * Creates a DockTabWidget copying other's DockTabWidget specific settings.
	 * @param other
	 * @param parent
	 */
	DockTabWidget(DockTabWidget *other, QWidget *parent = 0);
	
	bool contains(QWidget *widget) { return indexOf(widget) >= 0; }
	
	static void moveTab(DockTabWidget *source, int sourceIndex, DockTabWidget *dest, int destIndex);
	static void decodeTabDropEvent(QDropEvent *event, DockTabWidget **p_tabWidget, int *p_index);
	static bool eventIsTabDrag(QDragEnterEvent *event);
	
	virtual bool isInsertableFrom(DockTabWidget *other) { Q_UNUSED(other) return true; }
	virtual DockTabWidget *createAnother(QWidget *parent = 0);
	
signals:
	
	void activated();
	void willBeAutomaticallyDeleted(DockTabWidget *widget);
	
public slots:
	
	void activate() { emit activated(); }
	void deleteIfEmpty();
	
protected:
	
	void mousePressEvent(QMouseEvent *event);
	
private slots:
	
private:
};

class DockTabBar : public QTabBar
{
	Q_OBJECT
public:
	
	DockTabBar(DockTabWidget *tabWidget, QWidget *parent = 0);
	
	DockTabWidget *tabWidget() { return _tabWidget; }
	
signals:
	
	void activated();
	
protected:
	
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	
private:
	
	int insertionIndexAt(const QPoint &pos);
	
	DockTabWidget *_tabWidget = 0;
	bool _isStartingDrag = false;
	QPoint _dragStartPos;
};

}

#endif // DRAGGABLETABWIDGET_H
