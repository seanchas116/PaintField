#ifndef PAINTFIELD_CANVASTABWIDGET_H
#define PAINTFIELD_CANVASTABWIDGET_H

#include <QWidget>
#include "widgets/floatingdocktabwidget.h"

namespace PaintField {

class CanvasController;
class WorkspaceView;
class WorkspaceController;
class CanvasView;

class CanvasTabWidgetData;

class CanvasTabWidget : public FloatingDockTabWidget
{
	Q_OBJECT
public:
	
	typedef FloatingDockTabWidget super;
	
	CanvasTabWidget(WorkspaceView *workspaceView, QWidget *parent);
	CanvasTabWidget(CanvasTabWidget *other, QWidget *parent);
	~CanvasTabWidget();
	
	bool tabIsInsertable(DockTabWidget *other, int index) override;
	
	QObject *createNew() override;
	
	void memorizeTransforms();
	void restoreTransforms();
	
	void insertCanvas(int index, CanvasController *canvas);
	void addCanvas(CanvasController *canvas) { insertCanvas(count(), canvas); }
	
	QList<CanvasView *> canvasViews();
	
	WorkspaceController *workspace();
	
signals:
	
	void currentCanvasChanged(CanvasController *canvas);
	void activated();
	
public slots:
	
	void setCurrentCanvas(CanvasController *canvas);
	bool tryClose();
	void activate();
	
private slots:
	
	void onTabCloseRequested(int index);
	
private:
	
	CanvasView *canvasViewAt(int index);
	void commonInit();
	
	CanvasTabWidgetData *d;
};

class CanvasTabBar : public DockTabBar
{
	Q_OBJECT
	
public:
	
	CanvasTabBar(CanvasTabWidget *tabWidget, QWidget *parent = 0);
	
protected:
	
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	
private:
	
	CanvasTabWidget *_tabWidget;
};

} // namespace PaintField

#endif // PAINTFIELD_CANVASTABWIDGET_H
