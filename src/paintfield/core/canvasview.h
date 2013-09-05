#pragma once

#include <QObject>
#include <QWidget>
#include <Malachite/Surface>
#include <Malachite/Affine2D>
#include "canvasviewportsurface.h"

namespace PaintField {

class Tool;
class Canvas;

/// The CanvasView class provides canvas view "placeholders" in tab areas.
/// Actual views that receive events and draw canvases are created separately for performance.
class CanvasView : public QWidget
{
	Q_OBJECT
	
public:
	
	explicit CanvasView(Canvas *canvas, QWidget *parent = 0) :
		QWidget(parent),
		_canvas(canvas)
	{}
	
	Canvas *canvas() { return _canvas; }
	
signals:
	
	/// Emitted when resized
	void resized(const QSize &size);
	
	/// Emitted when parent changed, enabled changed or visibility changed
	void moved();
	
protected:
	
	void resizeEvent(QResizeEvent *);
	void changeEvent(QEvent *ev);
	void showEvent(QShowEvent *);
	void hideEvent(QHideEvent *);
	void paintEvent(QPaintEvent *);
	
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
	
	QWidget *viewport();

	CanvasViewportSurface mergedSurface() const;
	
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
	
	void onCurrentCanvasChanged(Canvas *canvas);
	
	void moveWidgets();
	
	/// set focus on the viewport and make canvas current
	void activate();
	void setFocus();
	
	void onViewWidgetSizeChanged();
	
private:
	
	void updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects);
	
	struct Data;
	Data *d;
};

} // namespace PaintField
