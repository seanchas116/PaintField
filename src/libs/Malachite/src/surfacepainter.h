#pragma once

//ExportName: SurfacePainter

#include "painter.h"

namespace Malachite
{

class MALACHITESHARED_EXPORT SurfacePainter : public Painter
{
public:
	SurfacePainter(Surface *surface) :
		Painter(surface) {}
	
	void setKeyClip(const QPointSet &keys);
	QPointSet keyClip() const;
	
	void setKeyRectClip(const QHash<QPoint, QRect> &keyRectClip);
	QHash<QPoint, QRect> keyRectClip() const;
};

}

