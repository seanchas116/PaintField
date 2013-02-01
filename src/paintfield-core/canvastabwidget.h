#ifndef PAINTFIELD_CANVASTABWIDGET_H
#define PAINTFIELD_CANVASTABWIDGET_H

#include <QWidget>
#include "workspacetabwidget.h"

namespace PaintField {

class Canvas;
class WorkspaceView;
class Workspace;
class CanvasView;

class CanvasTabWidget : public WorkspaceTabWidget
{
	Q_OBJECT
	
public:
	
	CanvasTabWidget(Workspace *workspace, QWidget *parent);
	~CanvasTabWidget();
	
	bool tabIsInsertable(DockTabWidget *other, int index) override;
	void insertTab(int index, QWidget *widget, const QString &title);
	
	QObject *createNew() override;
	
	void memorizeTransforms();
	void restoreTransforms();
	
	void insertCanvas(int index, Canvas *canvas);
	void addCanvas(Canvas *canvas) { insertCanvas(count(), canvas); }
	
	QList<CanvasView *> canvasViews();
	
signals:
	
	void currentCanvasChanged(Canvas *canvas);
	void activated();
	
public slots:
	
	bool tryClose();
	void activate();
	
private slots:
	
	void onTabCloseRequested(int index);
	void onCurrentCanvasChanged(Canvas *canvas);
	
	void onTabMovedIn(QWidget *widget);
	void onTabAboutToBeMovedOut(QWidget *widget);
	
private:
	
	CanvasView *canvasViewAt(int index);
	void commonInit();
	
	class Data;
	Data *d;
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
