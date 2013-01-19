#ifndef PAINTFIELD_CANVASTABWIDGET_H
#define PAINTFIELD_CANVASTABWIDGET_H

#include <QWidget>
#include "widgets/floatingdocktabwidget.h"

namespace PaintField {

class CanvasController;
class WorkspaceView;
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
	
	QList<CanvasView *> canvasViews();
	
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

} // namespace PaintField

#endif // PAINTFIELD_CANVASTABWIDGET_H
