#pragma once

#include "paintfield/core/tool.h"

namespace PaintField {

class RectTool : public Tool
{
	Q_OBJECT
	
public:
	
	explicit RectTool(Canvas *canvas);
	~RectTool();
	
	void tabletMoveEvent(CanvasTabletEvent *event);
	void tabletPressEvent(CanvasTabletEvent *event);
	void tabletReleaseEvent(CanvasTabletEvent *event);
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField
