#pragma once

#include "../paintengine.h"
#include "renderer.h"

namespace Malachite
{

class ImagePaintEngine : public PaintEngine
{
public: 
	
	ImagePaintEngine();
	
	bool begin(Paintable *paintable);
	bool flush();
	
	void drawPreTransformedPolygons(const FixedMultiPolygon &polygons);
	void drawPreTransformedImage(const QPoint &point, const Image &image, const QRect &imageMaskRect);
	
private:
	
	Bitmap<Pixel> _bitmap;
	Image *_image;
};

}

