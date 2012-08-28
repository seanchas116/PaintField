#ifndef FSTOOL_H
#define FSTOOL_H

#include <QObject>
#include <QString>
#include <QIcon>
#include <QGraphicsItem>
#include "fslayer.h"
#include "mlpainter.h"
#include "fscanvas.h"
#include "fstabletevent.h"
#include "fslayerrenderer.h"

class FSToolManager;

/**
  The FSTool object delegates editing of a layer in each FSCanvas.
  It is owned by a FSCanvas and reconstructed whenever a new FSCanvas is constucted (e.g., a new file is opened by the user).
*/
class FSTool : public QObject
{
	friend class FSToolManager;
	
	Q_OBJECT
public:
	
	explicit FSTool(FSCanvas *parent = 0) : QObject(parent) {}
	
	/**
	  Returns the index of the current layer.
	*/
	QModelIndex currentLayerIndex() { return document()->currentIndex(); }
	
	/**
	  Returns the pointer to the current layer.
	*/
	const FSLayer *currentLayer() { return document()->layerForIndex(currentLayerIndex()); }
	
	/**
	  Returns a graphics item which is displayed on top of the canvas.
	*/
	virtual QGraphicsItem *graphicsItem() { return 0; }
	
	
	/**
	  Returns a delegate which can be used in layer rendering.
	  Returning 0 meeans the object has no specific render delegate.
	*/
	virtual FSLayerRenderDelegate *renderDelegate() { return 0; }
	
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
	
	void requestUpdate(const QPointSet &tiles);
	
protected:
	
	FSDocumentModel *document() { return canvas()->document(); }
	FSCanvas *canvas() { return static_cast<FSCanvas *>(parent()); }
	
private:
};

class FSToolFactory : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString _name READ toolName WRITE setToolName)
	Q_PROPERTY(QString _text READ text WRITE setText)
	Q_PROPERTY(QIcon _icon READ icon WRITE setIcon)
	
public:
	
	explicit FSToolFactory(QObject *parent = 0) : QObject(parent) {}
	
	virtual FSTool *createTool(FSCanvas *parent) = 0;
	virtual QWidget *createSettingWidget() { return 0; }
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
