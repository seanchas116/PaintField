#pragma once

#include <QObject>

class QMouseEvent;
class QTabletEvent;
class QKeyEvent;

namespace PaintField {

class Tool;
class Canvas;

class CanvasToolEventFilter : public QObject
{
	Q_OBJECT
public:
	explicit CanvasToolEventFilter(Canvas *canvas, QObject *parent);
	~CanvasToolEventFilter();
	
	void setTool(Tool *tool);
	bool eventFilter(QObject *, QEvent *event) override;

private:
	
	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField
