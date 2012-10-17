#ifndef FSTOOL_H
#define FSTOOL_H

#include <QObject>
#include <QString>
#include <QIcon>
#include "document.h"
#include "Malachite/mlpainter.h"
#include "tabletevent.h"
#include "canvasview.h"
#include "layerrenderer.h"

class QGraphicsItem;

namespace PaintField
{

class ToolManager;

/**
  The FSTool object delegates editing of a layer in each FSCanvas.
  It is owned by a FSCanvas and reconstructed whenever a new FSCanvas is constucted (e.g., a new file is opened by the user).
*/
class Tool : public QObject
{
	friend class ToolManager;
	
	Q_OBJECT
public:
	
	explicit Tool(CanvasView *parent = 0) : QObject(parent) {}
	
	/**
	  Returns the index of the current layer.
	*/
	QModelIndex currentLayerIndex() { return document()->layerModel()->currentIndex(); }
	
	/**
	  Returns the pointer to the current layer.
	*/
	const Layer *currentLayer() { return document()->layerModel()->layerForIndex(currentLayerIndex()); }
	
	/**
	  Returns a graphics item which is displayed on top of the canvas.
	*/
	virtual QGraphicsItem *graphicsItem() { return 0; }
	
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
	
private:
	
	LayerConstList _customDrawLayers;
};

class ToolFactory : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText)
	Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
	
public:
	
	explicit ToolFactory(QObject *parent = 0) : QObject(parent) {}
	
	virtual Tool *createTool(CanvasView *parent) = 0;
	virtual QWidget *createSettingWidget() { return 0; }
	virtual bool isTypeSupported(Layer::Type type) const = 0;
	
	void setText(const QString &text) { _text = text; }
	QString text() const { return _text; }
	void setIcon(const QIcon &icon) { _icon = icon; }
	QIcon icon() const { return _icon; }
	
private:
	
	QString _name, _text;
	QIcon _icon;
};

}

#endif // FSTOOL_H
