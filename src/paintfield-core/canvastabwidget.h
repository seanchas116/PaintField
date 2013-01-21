#ifndef PAINTFIELD_CANVASTABWIDGET_H
#define PAINTFIELD_CANVASTABWIDGET_H

#include <QWidget>
#include "workspacetabwidget.h"

namespace PaintField {

class CanvasController;
class WorkspaceView;
class WorkspaceController;
class CanvasView;

class CanvasTabWidget : public WorkspaceTabWidget
{
	Q_OBJECT
	
public:
	
	CanvasTabWidget(WorkspaceController *workspace, QWidget *parent);
	~CanvasTabWidget();
	
	bool tabIsInsertable(DockTabWidget *other, int index) override;
	void insertTab(int index, QWidget *widget, const QString &title);
	
	QObject *createNew() override;
	
	void memorizeTransforms();
	void restoreTransforms();
	
	void insertCanvas(int index, CanvasController *canvas);
	void addCanvas(CanvasController *canvas) { insertCanvas(count(), canvas); }
	
	QList<CanvasView *> canvasViews();
	
signals:
	
	void currentCanvasChanged(CanvasController *canvas);
	void activated();
	
public slots:
	
	bool tryClose();
	void activate();
	
private slots:
	
	void onTabCloseRequested(int index);
	void onCurrentCanvasChanged(CanvasController *canvas);
	
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
