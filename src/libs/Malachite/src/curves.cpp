#include <QDebug>
#include "curves.h"

namespace Malachite
{

Curve4 Curve4::fromCatmullRom(const Polygon &polygon, int indexStart)
{
	if (indexStart < 0 || polygon.size() - 1 <= indexStart) {
		qWarning() << "MLCurve4::fromCatmullRom: index is invalid";
		return Curve4();
	}
	
	if (indexStart == 0) {
		return fromCatmullRomStart(polygon.at(0), polygon.at(1), polygon.at(2));
	}
	
	if (indexStart == polygon.size() - 2) {
		return fromCatmullRomEnd(polygon.at(indexStart - 1), polygon.at(indexStart), polygon.at(indexStart + 1));
	}
	
	return fromCatmullRom(polygon.at(indexStart - 1),
						  polygon.at(indexStart),
						  polygon.at(indexStart + 1),
						  polygon.at(indexStart + 2));
}

}

