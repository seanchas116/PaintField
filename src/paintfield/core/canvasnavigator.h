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

class CanvasNavigator : public QObject
{
	Q_OBJECT
public:
	explicit CanvasNavigator(KeyTracker *keyTracker, CanvasViewController *controller);
	~CanvasNavigator();
	
	enum DragMode
	{
		NoNavigation,
		Translating,
		Scaling,
		Rotating
	};
	
	DragMode dragMode() const;
	
	void mouseEvent(QMouseEvent *event);
	void tabletEvent(QTabletEvent *event);
	void customTabletEvent(WidgetTabletEvent *event);
	void wheelEvent(QWheelEvent *event);
	
signals:
	
	void clicked();
	
public slots:
	
private slots:
	
	void onPressedKeysChanged();
	
private:
	
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
	
	
	
	struct Data;
	Data *d;
};

} // namespace PaintField
