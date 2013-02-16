#ifndef PAINTFIELD_TABLETPOINTERDATA_H
#define PAINTFIELD_TABLETPOINTERDATA_H

#include <QTabletEvent>

namespace PaintField {

struct TabletPointerData
{
	TabletPointerData() :
	    id(0),
	    type(QTabletEvent::UnknownPointer)
	{}
	TabletPointerData(quint64 id, QTabletEvent::PointerType type) :
	    id(id),
	    type(type)
	{}
	TabletPointerData(const TabletPointerData &other) :
	    id(other.id),
	    type(other.type)
	{}
	
	quint64 id;
	QTabletEvent::PointerType type;
	
	bool operator==(const TabletPointerData &other)
	{
		return id == other.id && type == other.type;
	}
	
	bool operator!=(const TabletPointerData &other)
	{
		return !operator==(other);
	}
};

} // namespace PaintField

Q_DECLARE_METATYPE(PaintField::TabletPointerData)

#endif // PAINTFIELD_TABLETPOINTERDATA_H
