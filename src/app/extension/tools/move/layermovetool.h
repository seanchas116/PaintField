#ifndef FSLAYERMOVETOOL_H
#define FSLAYERMOVETOOL_H

#include <QObject>
#include "tool.h"
#include "layeredit.h"

namespace PaintField
{

class RasterLayer;

class LayerMoveTool : public Tool
{
	Q_OBJECT
public:
	explicit LayerMoveTool(Canvas *parent = 0);
	~LayerMoveTool();
	
	void drawLayer(MLSurfacePainter *painter, const Layer *layer);
	
signals:
	
public slots:
	
protected:
	
	void cursorPressEvent(TabletEvent *event);
	void cursorMoveEvent(TabletEvent *event);
	void cursorReleaseEvent(TabletEvent *event);
	
	void beginMoveLayer();
	void moveLayer(TabletEvent *event);
	void endMoveLayer();
	
private:
	
	const Layer *_layer;
	QPoint _dragStartPoint, _offset;
	QPointSet _lastKeys;
	bool _layerIsDragged;
};

class LayerMoveToolFactory : public ToolFactory
{
	Q_OBJECT
public:
	explicit LayerMoveToolFactory(QObject *parent = 0);
	
	Tool *createTool(Canvas *view) { return new LayerMoveTool(view); }
	bool isTypeSupported(Layer::Type type) const;
};

}

#endif // FSLAYERMOVETOOL_H
