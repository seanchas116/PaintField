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
	
	void currentCanvasChanged(CanvasController *canvas);
	
public slots:
	
	void addCanvas(CanvasController *canvas);
	void removeCanvas(CanvasController *canvas);
	void setCurrentCanvas(CanvasController *canvas);
	
	void splitVertically() { split(Qt::Vertical); }
	void splitHorizontally() { split(Qt::Horizontal); }
	void split(Qt::Orientation orientation);
	void closeCurrentSplit();
	
private slots:
	
	void onCurrentTabChanged(QWidget *tab);
	void onTabCloseRequested(QWidget *tab);
	void onCurrentCanvasPropertyChanged();
	
private:
	
	SplitTabAreaController *_tabArea = 0;
	CanvasController *_currentCanvas = 0;
};

}

#endif // WORKSPACECANVASAREACONTROLLER_H
