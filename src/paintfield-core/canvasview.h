#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QScrollArea>
#include "smartpointer.h"
#include "tabletevent.h"
#include "document.h"

namespace PaintField
{

class Tool;
class CanvasController;
class VanishingScrollBar;

class CanvasView : public QWidget
{
	Q_OBJECT
	
public:
	
	CanvasView(CanvasController *canvas, QWidget *parent = 0);
	~CanvasView();
	
	QTransform transformToScene() const { return _transformToScene; }
	QTransform transformFromScene() const { return _transformFromScene; }
	
	QTransform navigatorTransform() const { return _navigatorTransform; }
	
	double scale() const { return _scale; }
	double rotation() const { return _rotation; }
	QPoint translation() const { return _translation; }
	
	QPoint maxAbsTranslation() const { return _maxAbsTranslation; }
	
	CanvasController *controller() { return _canvas; }
	Document *document() { return _document; }
	LayerModel *layerModel() { return _document->layerModel(); }
	
	void setTool(Tool *tool);
	
public slots:
	
	/**
	 * Sets the scale of the canvas, with a central point on its center.
	 * translation is not changed.
	 * @param value
	 */
	void setScale(double value);
	
	/**
	 * Sets the scale of the canvas, with a central point on the center of the view.
	 * translation is changed proportionally.
	 * @param value
	 */
	void setViewScale(double value);
	
	/**
	 * Sets the rotation of the canvas, with a central point on its center.
	 * @param value
	 */
	void setRotation(double value);
	
	/**
	 * Sets the rotation of the canvas, with a central point on the center of the view.
	 * @param value
	 */
	void setViewRotation(double value);
	
	/**
	 * Sets the translation of the canvas.
	 * @param value
	 */
	void setTranslation(const QPoint &value);
	void setTranslation(int x, int y) { setTranslation(QPoint(x, y)); }
	
signals:
	
	void scaleChanged(double value);
	void rotationChanged(double value);
	void translationChanged(const QPoint &value);
	void maxAbsTranslationChanged(const QPoint &value);
	
	void resized(const QSize &size);
	
protected:
	
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void tabletEvent(QTabletEvent *event);
	void customTabletEvent(WidgetTabletEvent *event);
	
	void wheelEvent(QWheelEvent *event);
	
	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);
	
	bool event(QEvent *event);
	
private slots:
	
	void updateTiles(const QPointSet &keys) { updateTiles(keys, QHash<QPoint, QRect>()); }
	void updateTiles(const QHash<QPoint, QRect> &rects) { updateTiles(QPointSet(), rects); }
	
	void onScrollBarXChanged(int value);
	void onScrollBarYChanged(int value);
	
private:
	
	void updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects);
	
	bool sendCanvasMouseEvent(QMouseEvent *event);
	bool sendCanvasTabletEvent(QMouseEvent *mouseEvent);
	bool sendCanvasTabletEvent(WidgetTabletEvent *event);
	void updateTransforms();
	void updateScrollBarValue();
	void updateScrollBarRange();
	void moveScrollBars();
	
	VanishingScrollBar *_scrollBarX, *_scrollBarY;
	
	CanvasController *_canvas = 0;
	Document *_document = 0;
	
	ScopedQObjectPointer<Tool> _tool;
	
	double _mousePressure = 0;
	
	QPixmap _pixmap;
	
	QTransform _transformToScene, _transformFromScene, _navigatorTransform;
	
	double _scale = 1.0;
	double _rotation = 0.0;
	QPoint _translation;
	
	double _backupScale = 1.0;
	double _backupRotation = 0.0;
	QPoint _backupTranslation;
	
	QPoint _maxAbsTranslation;
};

}

#endif // CANVASVIEW_H
