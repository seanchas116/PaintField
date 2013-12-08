#pragma once

#include "tabletinputdata.h"
#include "global.h"
#include <QInputEvent>

namespace PaintField {

class CanvasCursorEvent : public QInputEvent
{
public:
	
	CanvasCursorEvent(int type, const Malachite::Vec2D &globalPos, const QPoint &globalPosInt, const Malachite::Vec2D &viewPos, const QPoint &viewPosInt, const TabletInputData &data, Qt::KeyboardModifiers keyState) :
		QInputEvent(static_cast<QEvent::Type>(type), keyState),
		globalPos(globalPos),
		globalPosInt(globalPosInt),
		viewPos(viewPos),
		viewPosInt(viewPosInt),
		data(data)
	{}
	
	Malachite::Vec2D globalPos;
	QPoint globalPosInt;
	Malachite::Vec2D viewPos;
	QPoint viewPosInt;
	TabletInputData data;
};

}
