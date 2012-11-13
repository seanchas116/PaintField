#ifndef FSTABLETEVENT_H
#define FSTABLETEVENT_H

#include <QInputEvent>

#include "global.h"
#include "tabletinputdata.h"

namespace PaintField {

class WidgetTabletEvent : public QInputEvent
{
public:
	
	WidgetTabletEvent(int type, const QPointF &globalPos, const QPoint &globalPosInt, const QPoint &pos, qreal pressure, int xTilt, int yTilt,
					  qreal rotation, qreal tangentialPressure, Qt::KeyboardModifiers keyState);
	
	QPoint posInt, globalPosInt;
	TabletInputData globalData;
};

class TabletEvent : public QInputEvent
{
public:
	TabletEvent(int type, const QPointF &globalPos, const QPoint &globalPosInt, const QPointF &posF, qreal pressure, int xTilt, int yTilt,
	              qreal rotation, qreal tangentialPressure, Qt::KeyboardModifiers keyState) :
		QInputEvent(static_cast<QEvent::Type>(type), keyState),
		globalPos(globalPos),
		globalPosInt(globalPosInt),
		data(posF, pressure, rotation, tangentialPressure, Malachite::Vec2D(xTilt, yTilt))
	{
		setAccepted(false);
	}
	
	Malachite::Vec2D globalPos;
	QPoint globalPosInt;
	TabletInputData data;
};

}

#endif // FSTABLETEVENT_H
