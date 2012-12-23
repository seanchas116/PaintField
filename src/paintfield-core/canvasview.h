#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include "widgets/navigatablearea.h"
#include "smartpointer.h"
#include "tabletevent.h"
#include "document.h"

namespace PaintField
{

class Tool;
class CanvasController;

class CanvasView : public NavigatableArea
{
	Q_OBJECT
	
	typedef NavigatableArea super;
	
public:
	
	CanvasView(CanvasController *canvas, QWidget *parent = 0);
	~CanvasView();
	
	CanvasController *controller() { return _canvas; }
	Document *document() { return _document; }
	LayerModel *layerModel() { return _document->layerModel(); }
	
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
	void onTabletActiveChanged(bool active) { _tabletActive = active; }
	
private:
	
	void updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects);
	
	bool sendCanvasMouseEvent(QMouseEvent *event);
	bool sendCanvasTabletEvent(QMouseEvent *mouseEvent);
	bool sendCanvasTabletEvent(WidgetTabletEvent *event);
	
	void addCustomCursorRectToRepaintRect();
	void addRepaintRect(const QRect &rect);
	void repaintDesignatedRect();
	
	CanvasController *_canvas = 0;
	Document *_document = 0;
	QPixmap _pixmap;
	ScopedQObjectPointer<Tool> _tool;
	
	double _mousePressure = 0;
	QRect _repaintRect;
	QRect _prevCustomCursorRect;
	Malachite::Vec2D _customCursorPos;
	bool _tabletActive = false;
};

}

#endif // CANVASVIEW_H
