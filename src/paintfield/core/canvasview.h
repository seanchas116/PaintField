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
	
	Malachite::Affine2D transformToScene() const;
	Malachite::Affine2D transformFromScene() const;
	
	QPoint viewCenter() const;
	
	bool isUpdateTilesEnabled() const;
	
	Canvas *canvas();
	
signals:
	
	void transformUpdated();
	
public slots:
	
	void setUpdateTilesEnabled(bool enable);
	void setTool(Tool *tool);
	
	void updateViewportAccurately();
	
private slots:
	
	void setScale(double value);
	void setRotation(double value);
	void setTranslation(const QPoint &value);
	void setMirrored(bool value);
	void setRetinaMode(bool value);
	
	void updateTiles(const QPointSet &keys) { updateTiles(keys, QHash<QPoint, QRect>()); }
	void updateTiles(const QHash<QPoint, QRect> &rects) { updateTiles(QPointSet(), rects); }
	
	void onClicked();
	
	void onScrollBarXChanged(int x);
	void onScrollBarYChanged(int y);
	void updateScrollBarRange();
	void updateScrollBarValue();
	
	void onCanvasWillBeDeleted();
	
private:
	
	void updateTransforms();
	
	void updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects);
	
	void moveWidgets();
	
	struct Data;
	Data *d;
};

} // namespace PaintField
