#ifndef FSDRAWUTIL_H
#define FSDRAWUTIL_H

#include <QPainter>
#include "mlsurface.h"

void fsDrawMLSurface(QPainter *painter, const QPoint &point, const MLSurface &surface);
void fsDrawMLImage(QPainter *painter, const QPoint &point, const MLImage &image);

inline void fsDrawMLSurface(QPainter *painter, int x, int y, const MLSurface &surface)
{
	fsDrawMLSurface(painter, QPoint(x, y), surface);
}

inline void fsDrawFSImage(QPainter *painter, int x, int y, const MLImage &image)
{
	fsDrawMLImage(painter, QPoint(x, y), image);
}


#endif // FSDRAWUTIL_H
