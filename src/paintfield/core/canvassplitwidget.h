#ifndef PAINTFIELD_CANVASSPLITWIDGET_H
#define PAINTFIELD_CANVASSPLITWIDGET_H

#include <QStackedWidget>
#include "interface.h"

namespace PaintField {

class CanvasTabWidget;

class CanvasSplitDefaultWidget: public QWidget, public DockTabDroppableInterface
{
	Q_OBJECT
	Q_INTERFACES(PaintField::DockTabDroppableInterface)
	
public:
	
	CanvasSplitDefaultWidget(CanvasTabWidget *tabWidget, QWidget *parent);
	
	bool dropDockTab(DockTabWidget *srcTabWidget, int srcIndex, const QPoint &pos) override;
	bool tabIsInsertable(DockTabWidget *src, int srcIndex) override;
	
signals:
	
	void activated();
	
protected:
	
	void mousePressEvent(QMouseEvent *);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	
private:
	
	CanvasTabWidget *_tabWidget;
};

class CanvasSplitWidget : public QStackedWidget
{
	Q_OBJECT
	
public:
	explicit CanvasSplitWidget(CanvasTabWidget *tabWidget, QWidget *parent = 0);
	
	CanvasTabWidget *tabWidget() { return _tabWidget; }
	
signals:
	
	void activated();
	
public slots:
	
	bool tryClose();
	
private slots:
	
	void onTabWidgetCurrentChanged(int index);
	
private:
	
	enum Index
	{
		IndexTabWidget = 0,
		IndexDefaultWidget = 1
	};
	
	CanvasTabWidget *_tabWidget;
	CanvasSplitDefaultWidget *_defaultWidget;
};

} // namespace PaintField

#endif // PAINTFIELD_CANVASSPLITWIDGET_H
