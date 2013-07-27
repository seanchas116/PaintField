#include "private/surfacepaintengine.h"

#include "surfacepainter.h"

namespace Malachite
{

void SurfacePainter::setKeyClip(const QPointSet &keys)
{
	static_cast<SurfacePaintEngine *>(paintEngine())->setKeyClip(keys);
}

QPointSet SurfacePainter::keyClip() const
{
	return static_cast<const SurfacePaintEngine *>(paintEngine())->keyClip();
}

void SurfacePainter::setKeyRectClip(const QHash<QPoint, QRect> &keyRectClip)
{
	static_cast<SurfacePaintEngine *>(paintEngine())->setKeyRectClip(keyRectClip);
}

QHash<QPoint, QRect> SurfacePainter::keyRectClip() const
{
	return static_cast<const SurfacePaintEngine *>(paintEngine())->keyRectClip();
}

}

