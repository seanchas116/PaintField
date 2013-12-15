#pragma once
#include "paintfield/core/tool.h"

namespace PaintField {

class SelectionRectTool : public Tool
{
	Q_OBJECT
public:

	enum Type
	{
		TypeRect,
		TypeEllipse
	};

	explicit SelectionRectTool(Type type, Canvas *parent = 0);
	~SelectionRectTool();

	int cursorPressEvent(CanvasCursorEvent *event) override;
	void cursorMoveEvent(CanvasCursorEvent *event, int type) override;
	void cursorReleaseEvent(CanvasCursorEvent *event, int type) override;

private:
	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField
