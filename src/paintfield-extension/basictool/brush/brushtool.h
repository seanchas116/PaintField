#ifndef FSBRUSHTOOL_H
#define FSBRUSHTOOL_H

#include <QObject>
#include "paintfield-core/tool.h"

namespace PaintField {

class TabletInput;
class BrushStroker;
class BrushStrokerFactory;
class BrushToolModule;
class BrushStrokingThread;

class BrushTool : public Tool
{
	Q_OBJECT
public:
	
	BrushTool(CanvasView *parent = 0);
	~BrushTool();
	
	void drawLayer(Malachite::SurfacePainter *painter, const Layer *layer);
	
	void drawCustomCursor(QPainter *painter, const Malachite::Vec2D &pos);
	QRect customCursorRect(const Malachite::Vec2D &pos);
	
signals:
	
public slots:
	
	void setColor(const Malachite::Color &color) { _argb = color.toArgb(); }
	void setStrokerFactory(BrushStrokerFactory *factory) { _strokerFactory = factory; }
	void setBrushSettings(const QVariantMap &settings);
	void setBrushSize(int size);
	
protected:
	
	void tabletPressEvent(CanvasTabletEvent *event);
	void tabletMoveEvent(CanvasTabletEvent *event);
	void tabletReleaseEvent(CanvasTabletEvent *event);
	
	bool canvasEventFilter(QEvent *event);
	
	void beginStroke(const TabletInput &data);
	void drawStroke(const TabletInput &data);
	void endStroke(const TabletInput &data);
	
	void updateTiles();
	
private:
	
	bool isStroking() const;
	void setPrevData(const TabletInput &data);
	
	Malachite::Vec4F _argb;
	BrushStrokerFactory *_strokerFactory = 0;
	QVariantMap _settings;
	int _brushSize;
	TabletInput _dataPrev;
	bool _dataPrevSet = false;
	const Layer *_layer = 0;
	
	BrushStrokingThread *_thread;
};

}

#endif // FSBRUSHTOOL_H
