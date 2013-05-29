#pragma once

#include <QWidget>
#include <Malachite/Surface>
#include "canvasviewportcontroller.h"

namespace PaintField {

class CanvasViewportNormal : public QWidget
{
	Q_OBJECT
public:
	explicit CanvasViewportNormal(QWidget *parent = 0);
	
	void setState(CanvasViewportState *state) { _state = state; }
	
protected:
	
	void paintEvent(QPaintEvent *event);
	
private:
	
	CanvasViewportState *_state = 0;
};

class CanvasViewportNormalWrapper
{
public:
	CanvasViewportNormalWrapper() :
	    _viewport(new CanvasViewportNormal())
	{
	}
	
	void setState(CanvasViewportState *state)
	{
		_viewport->setState(state);
	}
	
	void repaint(const QRect &rect)
	{
		_viewport->repaint(rect);
	}
	
	void update()
	{
		_viewport->update();
	}
	
	void placeViewport(QWidget *window)
	{
		_viewport->setParent(window);
	}

	void moveViewport(const QRect &rect, bool visible)
	{
		_viewport->setGeometry(rect);
		_viewport->show();
		_viewport->lower();
		_viewport->setVisible(visible);
	}
	
private:
	
	QScopedPointer<CanvasViewportNormal> _viewport;
};

} // namespace PaintField
