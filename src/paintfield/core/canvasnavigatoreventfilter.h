#pragma once

#include <QObject>

class QPoint;
class QMouseEvent;
class QTabletEvent;
class QWheelEvent;

namespace PaintField {

class WidgetTabletEvent;
class Canvas;
class KeyTracker;
class CanvasViewController;

class CanvasNavigatorEventFilter : public QObject
{
	Q_OBJECT
public:
	explicit CanvasNavigatorEventFilter(KeyTracker *keyTracker, Canvas *canvas, QObject *parent = 0);
	~CanvasNavigatorEventFilter();
	
	enum DragMode
	{
		NoNavigation,
		Translating,
		Scaling,
		Rotating
	};
	
	DragMode dragMode() const;
	bool eventFilter(QObject *object, QEvent *event) override;
	
signals:
	
	void clicked();
	
private:
	
	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField
