#ifndef CANVASTABAREACONTROLLER_H
#define CANVASTABAREACONTROLLER_H

#include <QObject>
#include <QPointer>
#include "util.h"
#include "widgets/tabdocumentarea.h"

namespace PaintField
{

class CanvasView;
class CanvasController;
class WorkspaceView;

class CanvasTabAreaController : public QObject
{
	Q_OBJECT
public:
	explicit CanvasTabAreaController(QObject *parent = 0);
	
	QWidget *createView(WorkspaceView *workspaceView, QWidget *parent = 0);
	
signals:
	
	void currentCanvasChanged(CanvasController *controller);
	
public slots:
	
	void addCanvas(CanvasController *controller);
	void removeCanvas(CanvasController *controller);
	void setCurrentCanvas(CanvasController *controller);
	
	void splitVertically() { split(Qt::Vertical); }
	void splitHorizontally() { split(Qt::Horizontal); }
	void closeCurrentSplit();
	
private slots:
	
	void onCurrentTabChanged(QWidget *tab);
	
private:
	
	CanvasView *canvasViewFromController(CanvasController *controller);
	
	void split(Qt::Orientation orientation);
	
	ScopedQObjectPointer<TabDocumentArea> _area;
	QList<CanvasView *> _canvasViews;
};

}

#endif // CANVASTABAREACONTROLLER_H
