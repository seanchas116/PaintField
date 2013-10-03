#pragma once

#include <QObject>

class QMouseEvent;
class QTabletEvent;
class QKeyEvent;

namespace PaintField {

class WidgetTabletEvent;
class Tool;
class CanvasViewController;

class CanvasToolEventSender : public QObject
{
	Q_OBJECT
public:
	explicit CanvasToolEventSender(CanvasViewController *controller);
	~CanvasToolEventSender();
	
	void setTool(Tool *tool);
	
	void keyEvent(QKeyEvent *event);
	void mouseEvent(QMouseEvent *event);
	void tabletEvent(QTabletEvent *event);
	void customTabletEvent(WidgetTabletEvent *event);
	
private:
	
	bool sendCanvasTabletEvent(QMouseEvent *mouseEvent);
	bool sendCanvasTabletEvent(QTabletEvent *event);
	bool sendCanvasTabletEvent(WidgetTabletEvent *event);
	
	struct Data;
	Data *d;
};

} // namespace PaintField
