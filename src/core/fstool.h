#ifndef FSTOOL_H
#define FSTOOL_H

#include <QObject>
#include <QString>
#include <QIcon>
#include <QGraphicsItem>
#include "fslayer.h"
#include "mlpainter.h"
#include "fscanvasview.h"
#include "fstabletevent.h"
#include "fslayerrenderer.h"

class FSToolManager;

/**
  The FSTool object delegates editing of a layer in each FSCanvas.
  It is owned by a FSCanvas and reconstructed whenever a new FSCanvas is constucted (e.g., a new file is opened by the user).
*/
class FSTool : public QObject, public FSLayerRenderDelegate
{
	friend class FSToolManager;
	
	Q_OBJECT
public:
	
	explicit FSTool(FSCanvasView *parent = 0) : QObject(parent), _delegatesRender(false) {}
	
	/**
	  Returns the index of the current layer.
	*/
	QModelIndex currentLayerIndex() { return documentModel()->currentIndex(); }
	
	/**
	  Returns the pointer to the current layer.
	*/
	const FSLayer *currentLayer() { return documentModel()->layerForIndex(currentLayerIndex()); }
	
	/**
	  Returns a graphics item which is displayed on top of the canvas.
	*/
	virtual QGraphicsItem *graphicsItem() { return 0; }
	
	/**
	  Renders the tile of the current layer specified by tileKey on painter.
	  A FSTool can delegate the FSCanvas object's rendering of the current layer using this function.
	*/
	void render(MLPainter *painter, const FSLayer *layer, const QPoint &tileKey) { Q_UNUSED(painter); Q_UNUSED(layer); Q_UNUSED(tileKey); }
	
	/**
	  Returns the current layer if delegatesRender() == true, otherwise 0.
	*/
	const FSLayer *target() { return _delegatesRender ? currentLayer() : 0; }
	
	/**
	  Returns whether this FSTool delegates rendering of the current layer.
	*/
	bool delegatesRender() { return _delegatesRender; }
	
	virtual void cursorPressEvent(FSTabletEvent *event) { event->ignore(); return; }
	virtual void cursorMoveEvent(FSTabletEvent *event) { event->ignore(); return; }
	virtual void cursorReleaseEvent(FSTabletEvent *event) { event->ignore(); return; }
	
	virtual void keyPressEvent(QKeyEvent *event) { event->ignore(); return; }
	virtual void keyReleaseEvent(QKeyEvent *event) { event->ignore(); return; }
	
	virtual void viewCursorPressEvent(QMouseEvent *event) { event->ignore(); return; }
	virtual void viewCursorMoveEvent(QMouseEvent *event) { event->ignore(); return; }
	virtual void viewCursorReleaseEvent(QMouseEvent *event) { event->ignore(); return; }
	
public slots:
	
signals:
	
protected:
	
	void setDelegatesRender(bool x) { _delegatesRender = x; }
	
	FSDocumentModel *documentModel() { return canvas()->documentModel(); }
	FSCanvas *canvas() { return canvasView()->canvas(); }
	FSCanvasView *canvasView() { return static_cast<FSCanvasView *>(parent()); }
	
private:
	
	bool _delegatesRender;
};

class FSToolFactory : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString _name READ toolName WRITE setToolName)
	Q_PROPERTY(QString _text READ text WRITE setText)
	Q_PROPERTY(QIcon _icon READ icon WRITE setIcon)
	
public:
	
	explicit FSToolFactory(QObject *parent = 0) : QObject(parent) {}
	
	virtual FSTool *createTool(FSCanvasView *view) = 0;
	virtual bool isTypeSupported(FSLayer::Type type) const = 0;
	
	void setToolName(const QString &name) { _name = name; }
	QString toolName() const { return _name; }
	void setText(const QString &text) { _text = text; }
	QString text() const { return _text; }
	void setIcon(const QIcon &icon) { _icon = icon; }
	QIcon icon() const { return _icon; }
	
private:
	
	QString _name, _text;
	QIcon _icon;
};


#endif // FSTOOL_H
