#ifndef PAINTFIELD_CANVASSPLITAREACONTROLLER_H
#define PAINTFIELD_CANVASSPLITAREACONTROLLER_H

#include <QObject>

class QSplitter;

namespace PaintField {

class CanvasController;
class CanvasSplitWidget;
class WorkspaceView;

class CanvasSplitAreaController : public QObject
{
	Q_OBJECT
public:
	explicit CanvasSplitAreaController(WorkspaceView *workspaceView, QObject *parent = 0);
	
	QWidget *view();
	
signals:
	
public slots:
	
	void splitCurrent(Qt::Orientation orientation);
	void splitCurrentVertically() { splitCurrent(Qt::Vertical); }
	void splitCurrentHorizontally() { splitCurrent(Qt::Horizontal); }
	
	void closeCurrent();
	
	void addCanvas(CanvasController *canvas);
	
private slots:
	
	void onSplitActivated();
	
private:
	
	CanvasSplitWidget *createSplitWidget();
	
	void addSplit(QWidget *existingSplit, QWidget *newSplit, Qt::Orientation orientation);
	void removeSplit(CanvasSplitWidget *split);
	
	void setCurrentSplit(QWidget *splitOrSplitter);
	
	QSplitter *splitterForWidget(QWidget *widget);
	
	WorkspaceView *_workspaceView;
	QSplitter *_rootSplitter;
	CanvasSplitWidget *_currentSplit;
};

} // namespace PaintField

#endif // PAINTFIELD_CANVASSPLITAREACONTROLLER_H
