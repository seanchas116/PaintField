#pragma once

#include <QObject>
#include <QWidget>
#include <Malachite/Surface>
#include <Malachite/Affine2D>

namespace PaintField {

class Tool;
class Canvas;

class CanvasView : public QWidget
{
	Q_OBJECT
	
public:
	
	explicit CanvasView(Canvas *canvas, QWidget *parent = 0) :
		QWidget(parent),
		_canvas(canvas)
	{}
	
	Canvas *canvas() { return _canvas; }
	
private:
	
	Canvas *_canvas;
};

class CanvasViewController : public QObject
{
	Q_OBJECT
public:
	
	explicit CanvasViewController(Canvas *canvas);
	~CanvasViewController();
	
	CanvasView *view();
	bool eventFilter(QObject *watched, QEvent *event) override;
	
	bool isUpdateTilesEnabled() const;
	
	Canvas *canvas();
	
	QSize viewSize();
	
signals:
	
	void viewSizeChanged(const QSize &size);
	
public slots:
	
	void setUpdateTilesEnabled(bool enable);
	void setTool(Tool *tool);
	
	void updateViewportAccurately();
	
private slots:
	
	void updateTiles(const QPointSet &keys) { updateTiles(keys, QHash<QPoint, QRect>()); }
	void updateTiles(const QHash<QPoint, QRect> &rects) { updateTiles(QPointSet(), rects); }
	
	void onClicked();
	
	void onScrollBarXChanged(int x);
	void onScrollBarYChanged(int y);
	void updateScrollBarRange();
	void updateScrollBarValue();
	
	void onCanvasWillBeDeleted();
	
	void onTransformUpdated();
	void onRetinaModeChanged(bool retinaMode);
	
	void onStrokingOrToolEditingChanged();
	
private:
	
	void updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects);
	void moveWidgets();
	
	struct Data;
	Data *d;
};

} // namespace PaintField
