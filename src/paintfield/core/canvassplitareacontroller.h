#pragma once

#include <QObject>

namespace PaintField {

class Canvas;
class CanvasSplitWidget;
class Workspace;

class MemorizableSplitter;

class CanvasSplitAreaController : public QObject
{
	Q_OBJECT
public:
	explicit CanvasSplitAreaController(Workspace *workspace, QObject *parent = 0);
	
	QWidget *view();
	
signals:
	
public slots:
	
	void splitCurrent(Qt::Orientation orientation);
	void splitCurrentVertically() { splitCurrent(Qt::Vertical); }
	void splitCurrentHorizontally() { splitCurrent(Qt::Horizontal); }
	
	void closeCurrent();
	
	void addCanvas(Canvas *canvas);
	
private slots:
	
	void onSplitActivated();
	
private:
	
	CanvasSplitWidget *createSplitWidget();
	
	void addSplit(CanvasSplitWidget *existingSplit, CanvasSplitWidget *newSplit, Qt::Orientation orientation);
	void removeSplit(CanvasSplitWidget *split);
	
	void setCurrentSplit(QWidget *splitOrSplitter);
	
	MemorizableSplitter *splitterForWidget(QWidget *widget);
	
	Workspace *_workspace;
	MemorizableSplitter *_rootSplitter;
	CanvasSplitWidget *_currentSplit;
};

} // namespace PaintField

