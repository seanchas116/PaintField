#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <Malachite/Affine2D>
#include <Malachite/Surface>
#include "global.h"
#include <QWidget>

namespace PaintField
{

class WidgetTabletEvent;
class Document;
class Tool;
class Canvas;
class LayerModel;

class CanvasView : public QWidget
{
	Q_OBJECT
	
	typedef QWidget super;
	
public:
	
	CanvasView(Canvas *canvas, QWidget *parent = 0);
	~CanvasView();
	
	Canvas *canvas();
	Document *document();
	LayerModel *layerModel();
	
	QPoint viewCenter() const;
	
	Malachite::Affine2D transformToScene() const;
	Malachite::Affine2D transformFromScene() const;
	
	bool isUpdateTilesEnabled() const;
	
public slots:
	
	void setUpdateTilesEnabled(bool enable);
	
	void setTool(Tool *tool);
	
protected:
	
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void tabletEvent(QTabletEvent *event);
	void customTabletEvent(WidgetTabletEvent *event);
	void focusInEvent(QFocusEvent *);
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	void wheelEvent(QWheelEvent *event);
	void resizeEvent(QResizeEvent *);
	void changeEvent(QEvent *event);
	void showEvent(QShowEvent *);
	void hideEvent(QHideEvent *);
	
	bool event(QEvent *event);
	
private slots:
	
	void setScale(double value);
	void setRotation(double value);
	void setTranslation(const QPoint &value);
	void setMirrored(bool mirrored);
	void setRetinaMode(bool mode);
	
	void updateTiles(const QPointSet &keys) { updateTiles(keys, QHash<QPoint, QRect>()); }
	void updateTiles(const QHash<QPoint, QRect> &rects) { updateTiles(QPointSet(), rects); }
	
	void onClicked();
	void onTabletActiveChanged(bool active);
	
	void onViewportAccurateUpdate();
	
	void onScrollBarXChanged(int x);
	void onScrollBarYChanged(int y);
	void updateScrollBarRange();
	void updateScrollBarValue();
	
private:
	
	void updateTransforms();
	
	void updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects);
	
	bool sendCanvasMouseEvent(QMouseEvent *event);
	bool sendCanvasTabletEvent(QMouseEvent *mouseEvent);
	bool sendCanvasTabletEvent(WidgetTabletEvent *event);
	
	void addRepaintRect(const QRect &rect);
	void repaintDesignatedRect();
	
	bool tryBeginDragNavigation(const QPoint &pos);
	bool continueDragNavigation(const QPoint &pos);
	void endDragNavigation();
	
	void beginDragTranslation(const QPoint &pos);
	void continueDragTranslation(const QPoint &pos);
	void endDragTranslation();
	
	void beginDragScaling(const QPoint &pos);
	void continueDragScaling(const QPoint &pos);
	void endDragScaling();
	
	void beginDragRotation(const QPoint &pos);
	void continueDragRotation(const QPoint &pos);
	void endDragRotation();
	
	void moveViewport();
	void moveScrollBars();
	
	enum DragNavigationMode
	{
		NoNavigation,
		Translating,
		Scaling,
		Rotating
	};
	
	
	class Data;
	Data *d;
};

}

#endif // CANVASVIEW_H
