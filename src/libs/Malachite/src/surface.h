#pragma once

//ExportName: Surface

#include <QHash>
#include <QList>
#include <QSet>
#include <QSharedData>

#include "global.h"
#include "color.h"
#include "image.h"
#include "misc.h"
#include "image.h"
#include "division.h"
#include "genericsurface.h"

namespace Malachite
{

class MALACHITESHARED_EXPORT Surface : public GenericSurface<Image>, public Paintable
{
public:
	
	typedef GenericSurface<Image> super;
	
	Surface() : super() {}
	Surface(const Surface &other) : super(other) {}
	
	PaintEngine *createPaintEngine() override;
};

class MALACHITESHARED_EXPORT SurfaceEditTracker
{
public:
	
	SurfaceEditTracker(Surface *surface) : _surface(surface) {}
	
	Image &tileRef(const QPoint &key)
	{
		_editedKeys << key;
		return _surface->tileRef(key);
	}
	
	QSet<QPoint> editedKeys() const
	{
		return _editedKeys;
	}
	
	Surface *surface() { return _surface; }
	const Surface *surface() const { return _surface; }
	
private:
	
	Surface *_surface;
	QSet<QPoint> _editedKeys;
};

MALACHITESHARED_EXPORT QDataStream &operator<<(QDataStream &out, const Surface &surface);
MALACHITESHARED_EXPORT QDataStream &operator>>(QDataStream &in, Surface &surfaceOut);

}

Q_DECLARE_METATYPE(Malachite::Surface)

