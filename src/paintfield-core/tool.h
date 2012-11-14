#ifndef FSTOOL_H
#define FSTOOL_H

#include <QObject>
#include <QString>
#include <QIcon>
#include <Malachite/Painter>
#include "document.h"
#include "tabletevent.h"
#include "canvasview.h"
#include "canvascontroller.h"
#include "layerrenderer.h"

class QGraphicsItem;

namespace PaintField
{

class ToolManager;

/**
 * The FSTool object delegates editing of a layer in each CanvasView.
 * It is owned by a CanvasView and recreated whenever the user change the current tool or a new canvas is created.
 */
class Tool : public QObject
{
	friend class ToolManager;
	
	Q_OBJECT
public:
	
	explicit Tool(CanvasView *parent = 0) : QObject(parent) {}
	
	/**
	 * @return The document's current layer index
	 */
	QModelIndex currentLayerIndex() { return canvasView()->controller()->selectionModel()->currentIndex(); }
	
	/**
	 * @return The document's current laye
	 */
	const Layer *currentLayer() { return document()->layerModel()->layerForIndex(currentLayerIndex()); }
	
	/**
	 * @return A graphics item which is displayed on top of the canvas
	 */
	virtual QGraphicsItem *graphicsItem() { return 0; }
	
	/**
	 * Draws a layer onto a painter.
	 * You do not need to apply opacity, blend mode or filters in this function when you override it.
	 * @param painter
	 * @param layer
	 */
	virtual void drawLayer(Malachite::SurfacePainter *painter, const Layer *layer) { Q_UNUSED(painter) Q_UNUSED(layer) }
	void addCustomDrawLayer(const Layer *layer) { _customDrawLayers << layer; }
	void clearCustomDrawLayer() { _customDrawLayers.clear(); }
	LayerConstList customDrawLayers() { return _customDrawLayers; }
	
	virtual void cursorPressEvent(TabletEvent *event) { event->ignore(); return; }
	virtual void cursorMoveEvent(TabletEvent *event) { event->ignore(); return; }
	virtual void cursorReleaseEvent(TabletEvent *event) { event->ignore(); return; }
	
	virtual void keyPressEvent(QKeyEvent *event) { event->ignore(); return; }
	virtual void keyReleaseEvent(QKeyEvent *event) { event->ignore(); return; }
	
public slots:
	
signals:
	
	void requestUpdate(const QPointSet &tiles);
	
protected:
	
	CanvasView *canvasView() { return static_cast<CanvasView *>(parent()); }
	Document *document() { return canvasView()->document(); }
	QItemSelectionModel *selectionModel() { return canvasView()->controller()->selectionModel(); }
	
private:
	
	LayerConstList _customDrawLayers;
};

}

#endif // FSTOOL_H
