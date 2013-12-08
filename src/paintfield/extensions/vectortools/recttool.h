#pragma once

#include "paintfield/core/tool.h"
#include "paintfield/core/rectlayer.h"

namespace PaintField {

class RectTool : public Tool
{
	Q_OBJECT
	
public:
	
	enum AddingType
	{
		/**
		 * Adding no layer.
		 */
		NoAdding,
		AddRect,
		AddEllipse,
		AddText
	};
	
	enum SelectingMode
	{
		/**
		 * Layer is selected immediately when clicked.
		 * Default if the adding type is NoAdding.
		 */
		SelectImmediately,
		
		/**
		 * Layer is selected later if drag distance is not enough.
		 * Otherwise layer is not selected and new layer is inserted.
		 * Default if the adding type is not NoAdding.
		 */
		SelectLater
	};
	
	explicit RectTool(AddingType type, Canvas *canvas);
	~RectTool();
	
	void drawLayer(Malachite::SurfacePainter *painter, const LayerConstRef &layer) override;
	
	void setSelectingMode(SelectingMode mode);
	SelectingMode selectingMode() const;
	
	AddingType addingType() const;
	
protected:
	
	void keyPressEvent(QKeyEvent *event);
	void cursorMoveEvent(CanvasCursorEvent *event, int id) override;
	int cursorPressEvent(CanvasCursorEvent *event) override;
	void cursorReleaseEvent(CanvasCursorEvent *event, int id) override;
	
private slots:
	
	void updateSelected();
	void updateLayer(const LayerConstRef &layer);
	void updateGraphicsItems();
	
private:
	
	enum Mode
	{
		NoOperation,
		Dragging,
		Inserting,
		MovingHandle
	};
	
	void updateHandles();
	void updateFrameRect();
	
	void onHandleMoved(const QPointF &pos, int handleFlags);
	void onHandleMoveFinished();
	void commit();
	
	void addHandle(int handleTypes, qreal zValue);
	
	void startAdding();
	void finishAdding();
	
	void selectLayer(const LayerConstRef &layer, bool isShiftPressed);
	
	friend class RectInserter;
	friend class RectHandleItem;
	
	struct Data;
	Data *d;
};

} // namespace PaintField
