#ifndef FSBRUSHTOOL_H
#define FSBRUSHTOOL_H

#include <QObject>
#include <QElapsedTimer>
#include <boost/optional.hpp>
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
	void setBrushSize(int size) { _brushSize = size; }
	void setSmoothEnabled(bool enabled) { _smoothEnabled = enabled; }
	
protected:
	
	void tabletPressEvent(CanvasTabletEvent *event);
	void tabletMoveEvent(CanvasTabletEvent *event);
	void tabletReleaseEvent(CanvasTabletEvent *event);
	
private slots:
	
	void updateTiles();
	void commitStroke();
	
private:
	
	void beginStroke(const TabletInputData &data);
	void drawStroke(const TabletInputData &data);
	void endStroke(const TabletInputData &data);
	
	BrushStrokerFactory *_strokerFactory = 0;
	QScopedPointer<BrushStroker> _stroker;
	
	Malachite::Pixel _pixel;
	QVariantMap _settings;
	bool _smoothEnabled = false;
	int _brushSize = 5;
	
	bool _isStroking = false;
	
	std::shared_ptr<const RasterLayer> _layer = 0;
	Malachite::Surface _surface;
	
	boost::optional<TabletInputData> _lastEndData;
	
	QTimer *_commitTimer = 0;
};

}

#endif // FSBRUSHTOOL_H
