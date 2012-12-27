#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include "widgets/navigatablearea.h"
#include "smartpointer.h"
#include "tabletevent.h"
#include "document.h"
#include "keytracker.h"

namespace PaintField
{

class Tool;
class CanvasController;

class CanvasViewData;

class CanvasView : public NavigatableArea
{
	Q_OBJECT
	
	typedef NavigatableArea super;
	
public:
	
	CanvasView(CanvasController *canvas, QWidget *parent = 0);
	~CanvasView();
	
	CanvasController *controller();
	Document *document();
	LayerModel *layerModel();
	
	void setTool(Tool *tool);
	
public slots:
	
signals:
	
protected:
	
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void tabletEvent(QTabletEvent *event);
	void customTabletEvent(WidgetTabletEvent *event);
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	
	void paintEvent(QPaintEvent *event);
	
	bool event(QEvent *event);
	
private slots:
	
	void updateTiles(const QPointSet &keys) { updateTiles(keys, QHash<QPoint, QRect>()); }
	void updateTiles(const QHash<QPoint, QRect> &rects) { updateTiles(QPointSet(), rects); }
	
	void onClicked();
	void onToolCursorChanged(const QCursor &cursor);
	void onTabletActiveChanged(bool active);
	
private:
	
	void updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects);
	
	bool sendCanvasMouseEvent(QMouseEvent *event);
	bool sendCanvasTabletEvent(QMouseEvent *mouseEvent);
	bool sendCanvasTabletEvent(WidgetTabletEvent *event);
	
	void addCustomCursorRectToRepaintRect();
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
	
	CanvasViewData *d;
};

}

#endif // CANVASVIEW_H
