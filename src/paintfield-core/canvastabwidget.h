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
	CanvasTabWidget(WorkspaceView *workspaceView, QWidget *parent);
	CanvasTabWidget(CanvasTabWidget *other, QWidget *parent);
	~CanvasTabWidget();
	
	bool tabIsInsertable(DockTabWidget *other, int index) override;
	QObject *createNew() override;
	
signals:
	
	void currentCanvasChanged(CanvasController *canvas);
	void activated();
	
public slots:
	
	void setCurrentCanvas(CanvasController *canvas);
	
	bool tryClose();
	
private slots:
	
	void onActivated();
	void onCurrentIndexChanged(int index);
	void onTabCloseRequested(int index);
	
private:
	
	CanvasView *canvasViewAt(int index);
	void commonInit();
	
	CanvasTabWidgetData *d;
};

} // namespace PaintField

#endif // PAINTFIELD_CANVASTABWIDGET_H
