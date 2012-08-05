#ifndef FSBRUSHTOOL_H
#define FSBRUSHTOOL_H

#include <QObject>
#include "fstool.h"
#include "fscanvas.h"

class FSBrushStroker;
class FSTabletInputData;

class FSBrushTool : public FSTool
{
	Q_OBJECT
public:
	explicit FSBrushTool(FSCanvasView *parent = 0);
	~FSBrushTool();
	
	void render(MLPainter *painter, const FSLayer *layer, const QPoint &tileKey);
	
signals:
	
public slots:
	
protected:
	
	void cursorPressEvent(FSTabletEvent *event);
	void cursorMoveEvent(FSTabletEvent *event);
	void cursorReleaseEvent(FSTabletEvent *event);
	
	bool canvasEventFilter(QEvent *event);
	
	void beginStroke(const FSTabletInputData &data);
	void drawStroke(const FSTabletInputData &data);
	void endStroke();
	
private:
	QScopedPointer<FSBrushStroker> _stroker;
	const FSLayer *_layer;
	MLSurface _surface;
	int _inputCount;
};

class FSBrushToolFactory : public FSToolFactory
{
	Q_OBJECT
public:
	explicit FSBrushToolFactory(QObject *parent = 0);
	
	FSTool *createTool(FSCanvasView *view) { return new FSBrushTool(view); }
	bool isTypeSupported(FSLayer::Type type) const;
	
signals:
	
public slots:
	
};

#endif // FSBRUSHTOOL_H
