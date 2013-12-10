#pragma once
#include "paintfield/core/tool.h"

namespace PaintField {

class SelectionPaintTool : public Tool
{
	Q_OBJECT
public:

	enum Type
	{
		TypeBrush,
		TypeEraser
	};

	explicit SelectionPaintTool(Type type, Canvas *parent = 0);
	~SelectionPaintTool();

	int cursorPressEvent(CanvasCursorEvent *event) override;
	void cursorMoveEvent(CanvasCursorEvent *event, int id) override;
	void cursorReleaseEvent(CanvasCursorEvent *event, int id) override;

private:

	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField
