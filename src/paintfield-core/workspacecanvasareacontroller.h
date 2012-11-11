#ifndef WORKSPACECANVASAREACONTROLLER_H
#define WORKSPACECANVASAREACONTROLLER_H

#include <QObject>

namespace PaintField
{

class CanvasController;
class WorkspaceView;
class SplitTabAreaController;

class WorkspaceCanvasAreaController : public QObject
{
	Q_OBJECT
public:
	explicit WorkspaceCanvasAreaController(QWidget *baseWindow, QObject *parent = 0);
	
	QWidget *view();
	
signals:
	
	void currentCanvasChanged(CanvasController *controller);
	
public slots:
	
	void addCanvas(CanvasController *controller);
	void removeCanvas(CanvasController *controller);
	void setCurrentCanvas(CanvasController *controller);
	
	void splitVertically() { split(Qt::Vertical); }
	void splitHorizontally() { split(Qt::Horizontal); }
	void split(Qt::Orientation orientation);
	void closeCurrentSplit();
	
private slots:
	
	void onCurrentTabChanged(QWidget *tab);
	
private:
	
	SplitTabAreaController *_tabArea;
};

}

#endif // WORKSPACECANVASAREACONTROLLER_H
