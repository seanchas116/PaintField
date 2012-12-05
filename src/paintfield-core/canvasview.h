#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QScrollArea>
#include "tabletevent.h"
#include "document.h"

namespace PaintField
{

class Tool;
class CanvasController;

class CanvasView : public QWidget
{
	Q_OBJECT
	
public:
	
	CanvasView(CanvasController *canvas, QWidget *parent = 0);
	
	QTransform transformToScene() const { return _transformToScene; }
	QTransform transformFromScene() const { return _transformFromScene; }
	
	QTransform navigatorTransform() const { return _navigatorTransform; }
	
	double scale() const { return _scale; }
	double rotation() const { return _rotation; }
	QPoint translation() const { return _translation; }
	
	CanvasController *controller() { return _canvas; }
	Document *document() { return _document; }
	LayerModel *layerModel() { return _document->layerModel(); }
	
public slots:
	
	void setScale(double value);
	void setRotation(double value);
	void setTranslation(const QPoint &value);
	
	void setTool(const QString &name);
	void setTool(Tool *tool);
	
signals:
	
	void scaleChanged(double value);
	void rotationChanged(double value);
	void translationChanged(const QPoint &value);
	
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
	
	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);
	
	bool event(QEvent *event);
	
private slots:
	
	void updateTiles(const QPointSet &keys) { updateTiles(keys, QHash<QPoint, QRect>()); }
	void updateTiles(const QHash<QPoint, QRect> &rects) { updateTiles(QPointSet(), rects); }
	
private:
	
	void updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects);
	
	bool sendCanvasMouseEvent(QMouseEvent *event);
	bool sendCanvasTabletEvent(QMouseEvent *mouseEvent);
	bool sendCanvasTabletEvent(WidgetTabletEvent *event);
	void updateTransforms();
	
	CanvasController *_canvas = 0;
	Document *_document = 0;
	Tool *_tool = 0;
	double _mousePressure = 0;
	QPixmap _pixmap;
	QTransform _transformToScene, _transformFromScene, _navigatorTransform;
	double _scale;
	double _rotation;
	QPoint _translation;
};

}

#endif // CANVASVIEW_H
