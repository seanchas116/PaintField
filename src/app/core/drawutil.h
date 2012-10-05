#ifndef FSDRAWUTIL_H
#define FSDRAWUTIL_H

#include <QPainter>
#include "Malachite/mlsurface.h"

namespace PaintField {

void drawMLSurface(QPainter *painter, const QPoint &point, const Malachite::Surface &surface);
void drawMLImage(QPainter *painter, const QPoint &point, const Malachite::Image &image);

// pixel count of image must be multiple of 4
void drawMLImageFast(QPainter *painter, const QPoint &point, const Malachite::Image &image);

inline void drawMLSurface(QPainter *painter, int x, int y, const Malachite::Surface &surface)
{
	drawMLSurface(painter, QPoint(x, y), surface);
}

inline void drawMLImage(QPainter *painter, int x, int y, const Malachite::Image &image)
{
	drawMLImage(painter, QPoint(x, y), image);
}

}

#endif // FSDRAWUTIL_H
