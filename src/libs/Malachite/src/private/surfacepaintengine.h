#pragma once

#include <QPaintEngine>
#include "../surface.h"
#include "../paintengine.h"

namespace Malachite
{

class SurfacePaintEngine : public PaintEngine
{
public:
	
	SurfacePaintEngine();
	~SurfacePaintEngine();
	
	bool begin(Paintable *paintable);
	bool flush();
	
	void drawPreTransformedPolygons(const FixedMultiPolygon &polygons);
	void drawPreTransformedImage(const QPoint &point, const Image &image);
	void drawPreTransformedImage(const QPoint &point, const Image &image, const QRect &imageMaskRect);
	
	void drawPreTransformedSurface(const QPoint &point, const Surface &surface);
	
	void setKeyClip(const QPointSet &keys) { _keyClip = keys; }
	QPointSet keyClip() const { return _keyClip; }
	
	void setKeyRectClip(const QHash<QPoint, QRect> &keyRectClip) { _keyRectClip = keyRectClip; _keyClip = keyRectClip.keys().toSet(); }
	QHash<QPoint, QRect> keyRectClip() const { return _keyRectClip; }
	
private:
	
	Surface *_surface = 0;
	QPointSet _keyClip;
	QHash<QPoint, QRect> _keyRectClip;
};

}

