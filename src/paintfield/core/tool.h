#pragma once

#include <QObject>
#include <QString>
#include <QIcon>
#include <Malachite/Painter>
#include "document.h"
#include "canvascursorevent.h"
#include "canvasview.h"
#include "canvas.h"
#include "layerrenderer.h"

class QGraphicsItem;

namespace PaintField
{

class ToolManager;

/**
 * The Tool object delegates editing of a layer in each CanvasView.
 * It is owned by a CanvasView and recreated whenever the user change the current tool or a new canvas is created.
 */
class Tool : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QCursor cursor READ cursor)
	Q_PROPERTY(bool editing READ isEditing NOTIFY editingChanged)
	
public:
	
	struct LayerInsertion
	{
		LayerConstRef parent;
		int index;
		LayerRef layer;
	};

	enum CursorId
	{
		CursorIdDefault = -1
	};
	
	explicit Tool(Canvas *parent = 0);
	~Tool();
	
	/**
	 * @return The document's current layer
	 */
	LayerConstRef currentLayer();
	
	/**
	 * @return A graphics item which is displayed on top of the canvas
	 */
	QGraphicsItem *graphicsItem();
	
	/**
	 * Draws a layer onto a painter.
	 * You do not need to apply opacity, blend mode or filters in this function when you override it.
	 * @param painter
	 * @param layer
	 */
	virtual void drawLayer(Malachite::SurfacePainter *painter, const LayerConstRef &layer) { Q_UNUSED(painter) Q_UNUSED(layer) }
	
	/**
	 * Adds a new layer insertion which is performed on rendering.
	 * The Tool takes ownership of "layer".
	 * @param parent Where the layer is inserted
	 * @param index The index
	 * @param layer The inserted layer
	 */
	void addLayerInsertion(const LayerConstRef &parent, int index, const LayerRef &layer);
	
	void clearLayerInsertions();
	
	QList<LayerInsertion> layerInsertions() const;
	
	/**
	 * Adds a layer delegation which is performed on rendering.
	 * Tool::drawLayer is called instead of the default layer drawing function of canvas, when the layer is going to be rendered.
	 * @param layer
	 */
	void addLayerDelegation(const LayerConstRef &layer);
	
	void clearLayerDelegation();
	
	QList<LayerConstRef> layerDelegations() const;
	
	QCursor cursor() const;

	virtual int cursorPressEvent(CanvasCursorEvent *event) = 0;
	virtual void cursorMoveEvent(CanvasCursorEvent *event, int id) = 0;
	virtual void cursorReleaseEvent(CanvasCursorEvent *event, int id) = 0;
	
	virtual void keyPressEvent(QKeyEvent *event) { event->ignore(); return; }
	virtual void keyReleaseEvent(QKeyEvent *event) { event->ignore(); return; }
	
	virtual void toolEvent(QEvent *event);
	
	bool isEditing() const;
	SelectionShowMode selectionShowMode() const;
	
public slots:
	
signals:
	
	void requestUpdate(const QPointSet &tiles);
	void requestUpdate(const QHash<QPoint, QRect> &rects);
	
	void editingChanged(bool editing);
	
protected:

	void setEditing(bool editing);
	void setSelectionShowMode(SelectionShowMode mode);
	void setCursor(const QCursor &cursor);
	void setGraphicsItem(QGraphicsItem *item);
	
	Canvas *canvas() { return static_cast<Canvas *>(parent()); }
	Document *document() { return canvas()->document(); }
	LayerScene *layerScene() { return document()->layerScene(); }

	int currentCursorId() const;
	
private:
	
	struct Data;
	QScopedPointer<Data> d;
};

}
