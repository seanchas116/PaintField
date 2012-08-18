#ifndef FSBRUSHTOOL_H
#define FSBRUSHTOOL_H

#include <QObject>
#include "fstool.h"
#include "fscanvas.h"
#include "fsbrushsetting.h"

class FSBrushStroker;
class FSTabletInputData;

class FSBrushTool : public FSTool
{
	Q_OBJECT
public:
	explicit FSBrushTool(FSCanvasView *parent = 0);
	~FSBrushTool();
	
	void render(MLPainter *painter, const FSLayer *layer, const QPoint &tileKey);
	
	void setBrushSetting(const FSBrushSetting *setting) { _brushSetting = setting; }
	const FSBrushSetting *brushSetting() const { return _brushSetting; }
	
signals:
	
public slots:
	
protected:
	
	void cursorPressEvent(FSTabletEvent *event);
	void cursorMoveEvent(FSTabletEvent *event);
	void cursorReleaseEvent(FSTabletEvent *event);
	
	bool canvasEventFilter(QEvent *event);
	
	void beginStroke(const FSTabletInputData &data);
	void drawStroke(const FSTabletInputData &data);
	void endStroke(const FSTabletInputData &data);
	
	void updateTiles();
	
private:
	
	void setPrevData(const FSTabletInputData &data);
	
	QScopedPointer<FSBrushStroker> _stroker;
	FSTabletInputData _dataPrev, _dataBeforePrev;
	bool _dataPrevSet, _trailing;
	const FSBrushSetting *_brushSetting;
	const FSLayer *_layer;
	MLSurface _surface;
};

class FSBrushToolFactory : public FSToolFactory
{
	Q_OBJECT
public:
	explicit FSBrushToolFactory(QObject *parent = 0);
	
	FSTool *createTool(FSCanvasView *view);
	bool isTypeSupported(FSLayer::Type type) const;
	
	FSBrushSetting *setting() { return &_setting; }
	const FSBrushSetting *setting() const { return &_setting; }
	
signals:
	
public slots:
	
private:
	FSBrushSetting _setting;
};

#endif // FSBRUSHTOOL_H
