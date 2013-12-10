#pragma once

#include <QPainter>
#include <Malachite/Surface>

namespace PaintField {

namespace DrawUtil
{

void drawMLSurface(QPainter *painter, const QPoint &point, const Malachite::Surface &surface);
void drawMLImage(QPainter *painter, const QPoint &point, const Malachite::Image &image);

inline void drawMLSurface(QPainter *painter, int x, int y, const Malachite::Surface &surface)
{
	drawMLSurface(painter, QPoint(x, y), surface);
}

inline void drawMLImage(QPainter *painter, int x, int y, const Malachite::Image &image)
{
	drawMLImage(painter, QPoint(x, y), image);
}

} // namespace DrawUtil

} // namespace PaintField

