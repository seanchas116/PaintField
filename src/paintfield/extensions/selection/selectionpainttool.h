#pragma once
#include "paintfield/core/tool.h"

namespace PaintField {

class SelectionPaintTool : public Tool
{
	Q_OBJECT
	Q_PROPERTY(double brushSize READ brushSize WRITE setBrushSize NOTIFY brushSizeChanged)
	
public:

	enum Type
	{
		TypeBrush,
		TypeEraser
	};

	explicit SelectionPaintTool(Type type, Canvas *parent);
	~SelectionPaintTool();

	int cursorPressEvent(CanvasCursorEvent *event) override;
	void cursorMoveEvent(CanvasCursorEvent *event, int id) override;
	void cursorReleaseEvent(CanvasCursorEvent *event, int id) override;
	
	void setBrushSize(double size);
	double brushSize() const;
	
signals:
	void brushSizeChanged(double size);
	
private:

	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField
