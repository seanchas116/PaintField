#pragma once

#include <QPixmap>
#include "global.h"

class QWidget;

namespace PaintField {

struct CanvasViewportState;

class CanvasViewportCocoaWrapper
{
public:
	
	CanvasViewportCocoaWrapper();
	~CanvasViewportCocoaWrapper();
	
	void setState(CanvasViewportState *state);
	void repaint(const QRect &rect);
	void update();
	void placeViewport(QWidget *window);
	void moveViewport(const QRect &rect, bool visible);
	
private:
	
	void *_viewport;
};

}

