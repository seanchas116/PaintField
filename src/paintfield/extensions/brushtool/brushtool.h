#ifndef FSBRUSHTOOL_H
#define FSBRUSHTOOL_H

#include <QObject>
#include <QElapsedTimer>
#include "paintfield/core/tool.h"

class QTimer;

namespace PaintField {

struct TabletInputData;
class BrushStroker;
class BrushStrokerFactory;
class BrushToolExtension;

class BrushTool : public Tool
{
	Q_OBJECT
public:
	
	BrushTool(Canvas *parent = 0);
	~BrushTool();
	
	void drawLayer(Malachite::SurfacePainter *painter, const LayerConstPtr &layer);
	
	void drawCustomCursor(QPainter *painter, const Malachite::Vec2D &pos);
	QRect customCursorRect(const Malachite::Vec2D &pos);
	
signals:
	
public slots:
	
	void setColor(const Malachite::Color &color) { _pixel = color.toPixel(); }
	void setStrokerFactory(BrushStrokerFactory *factory) { _strokerFactory = factory; }
	void setBrushSettings(const QVariantMap &settings);
	void setBrushSize(int size);
	
protected slots:
	
	void updateTiles();
	
protected:
	
	void tabletPressEvent(CanvasTabletEvent *event);
	void tabletMoveEvent(CanvasTabletEvent *event);
	void tabletReleaseEvent(CanvasTabletEvent *event);
	
	bool canvasEventFilter(QEvent *event);
	
	void beginStroke(const TabletInputData &data);
	void drawStroke(const TabletInputData &data);
	void endStroke(const TabletInputData &data);
	
private:
	
	bool isStroking() const { return _stroker; }
	void setPrevData(const TabletInputData &data);
	
	Malachite::Pixel _pixel;
	BrushStrokerFactory *_strokerFactory = 0;
	QScopedPointer<BrushStroker> _stroker;
	QVariantMap _settings;
	int _brushSize;
	TabletInputData _dataPrev;
	bool _dataPrevSet = false;
	std::shared_ptr<const RasterLayer> _layer = 0;
	Malachite::Surface _surface;
};

}

#endif // FSBRUSHTOOL_H
