#ifndef FSTABLETEVENT_H
#define FSTABLETEVENT_H

#include <QInputEvent>

#include "global.h"
#include "tabletinputdata.h"

namespace PaintField {

class WidgetTabletEvent : public QInputEvent
{
public:
	
	WidgetTabletEvent(int type, const QPoint &globalPosInt, const QPoint &posInt, const TabletInputData &globalData, Qt::KeyboardModifiers keyState) :
		QInputEvent(static_cast<QEvent::Type>(type), keyState),
		posInt(posInt),
		globalPosInt(globalPosInt),
		globalData(globalData)
	{}
	
	QPoint posInt, globalPosInt;
	TabletInputData globalData;
};

class CanvasTabletEvent : public QInputEvent
{
public:
	
	CanvasTabletEvent(int type, const Malachite::Vec2D &globalPos, const QPoint &globalPosInt, const TabletInputData &data, Qt::KeyboardModifiers keyState) :
		QInputEvent(static_cast<QEvent::Type>(type), keyState),
		globalPos(globalPos),
		globalPosInt(globalPosInt),
		data(data)
	{}
	
	Malachite::Vec2D globalPos;
	QPoint globalPosInt;
	TabletInputData data;
};

class CanvasMouseEvent : public QInputEvent
{
public:
	
	CanvasMouseEvent(int type, const QPoint &globalPos, const Malachite::Vec2D &pos, Qt::KeyboardModifiers keyState) :
		QInputEvent(static_cast<QEvent::Type>(type), keyState),
		globalPos(globalPos),
		pos(pos)
	{}
	
	QPoint globalPos;
	Malachite::Vec2D pos;
};




}

#endif // FSTABLETEVENT_H
