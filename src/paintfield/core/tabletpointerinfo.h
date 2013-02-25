#pragma once

#include <QTabletEvent>
#include <QHash>

namespace PaintField {

struct TabletPointerInfo
{
	TabletPointerInfo() :
	    id(0),
	    type(QTabletEvent::UnknownPointer)
	{}
	TabletPointerInfo(quint64 id, QTabletEvent::PointerType type) :
	    id(id),
	    type(type)
	{}
	TabletPointerInfo(const TabletPointerInfo &other) :
	    id(other.id),
	    type(other.type)
	{}
	
	quint64 id;
	QTabletEvent::PointerType type;
	
	bool operator==(const TabletPointerInfo &other) const
	{
		return id == other.id && type == other.type;
	}
	
	bool operator!=(const TabletPointerInfo &other) const { return !operator==(other); }
};

inline uint qHash(const PaintField::TabletPointerInfo &pointerInfo)
{
	return qHash(QPair<quint64, int>(pointerInfo.id, int(pointerInfo.type)));
}


} // namespace PaintField


Q_DECLARE_METATYPE(PaintField::TabletPointerInfo)
