#ifndef FSLAYERMOVETOOL_H
#define FSLAYERMOVETOOL_H

#include <QObject>
#include "paintfield-core/tool.h"
#include "paintfield-core/layeredit.h"

namespace PaintField
{

class RasterLayer;

class LayerMoveTool : public Tool
{
	Q_OBJECT
public:
	explicit LayerMoveTool(CanvasView *parent = 0);
	
	void drawLayer(Malachite::SurfacePainter *painter, const Layer *layer);
	
signals:
	
public slots:
	
protected:
	
	void tabletPressEvent(CanvasTabletEvent *event);
	void tabletMoveEvent(CanvasTabletEvent *event);
	void tabletReleaseEvent(CanvasTabletEvent *event);
	
	void beginMoveLayer();
	void moveLayer(CanvasTabletEvent *event);
	void endMoveLayer();
	
private:
	
	const Layer *_layer = 0;
	QPoint _dragStartPoint, _offset;
	QPointSet _lastKeys;
	bool _layerIsDragged = false;
};

}

#endif // FSLAYERMOVETOOL_H
