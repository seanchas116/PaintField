#ifndef FSTABLETEVENT_H
#define FSTABLETEVENT_H

#include <QInputEvent>

#include "fsglobal.h"
#include "fstabletinputdata.h"

class FSTabletEvent : public QInputEvent
{
public:
	FSTabletEvent(int type, const QPointF &globalPos, const QPoint &globalPosInt, const QPointF &posF, qreal pressure, int xTilt, int yTilt,
	              qreal rotation, qreal tangentialPressure, Qt::KeyboardModifiers keyState) :
		QInputEvent(static_cast<QEvent::Type>(type), keyState),
		globalPos(globalPos),
		globalPosInt(globalPosInt),
		data(posF, pressure, rotation, tangentialPressure, MLVec2D(xTilt, yTilt))
	{
		setAccepted(false);
	}
	
	MLVec2D globalPos;
	QPoint globalPosInt;
	FSTabletInputData data;
};

#endif // FSTABLETEVENT_H
