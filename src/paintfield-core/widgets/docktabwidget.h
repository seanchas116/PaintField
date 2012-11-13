#ifndef DRAGGABLETABWIDGET_H
#define DRAGGABLETABWIDGET_H

#include <QTabBar>
#include <QTabWidget>
#include "../util.h"

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
	
	static void moveTab(DockTabWidget *source, int sourceIndex, DockTabWidget *dest, int destIndex);
	static void decodeTabDropEvent(QDropEvent *event, DockTabWidget **p_tabWidget, int *p_index);
	static bool eventIsTabDrag(QDragEnterEvent *event);
	
	virtual bool isInsertableFrom(DockTabWidget *other) { Q_UNUSED(other) return true; }
	virtual void insertTab(int index, QWidget *widget, const QString &title) { QTabWidget::insertTab(index, widget, title); }
	//virtual DockTabWidget *createAnother(QWidget *parent = 0);
	
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
	
	void focusInEvent(QFocusEvent *);
	void focusOutEvent(QFocusEvent *);
	void closeEvent(QCloseEvent *event);
	
private slots:
	
private:
	
	bool _autoDeletionEnabled = false;
};

class DockTabBar : public QTabBar
{
	Q_OBJECT
public:
	
	DockTabBar(DockTabWidget *tabWidget, QWidget *parent = 0);
	
	DockTabWidget *tabWidget() { return _tabWidget; }
	
signals:
	
	void clicked();
	
protected:
	
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	
	void dragMoveEvent(QDragMoveEvent *event);
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
