#pragma once

//ExportName: PaintEngine

#include <QPainterPath>
#include <QStack>
#include "blendmode.h"
#include "paintable.h"
#include "image.h"
#include "surface.h"
#include "brush.h"
#include "fixedpolygon.h"

namespace Malachite
{

struct PaintEngineState
{
	BlendMode blendMode;
	Brush brush;
	double opacity;
	QTransform shapeTransform;
	Malachite::ImageTransformType imageTransformType;
};

class MALACHITESHARED_EXPORT PaintEngine
{
public:
	PaintEngine();
	virtual ~PaintEngine() {}
	
	virtual bool begin(Paintable *paintable) = 0;
	virtual bool flush() = 0;
	
	virtual void drawPreTransformedImage(const QPoint &point, const Image &image, const QRect &imageMaskRect) = 0;
	virtual void drawPreTransformedSurface(const QPoint &point, const Surface &surface);
	virtual void drawPreTransformedPolygons(const FixedMultiPolygon &polygons) = 0;
	
	virtual void drawPreTransformedImage(const QPoint &point, const Image &image) { drawPreTransformedImage(point, image, image.rect()); }
	virtual void drawPreTransformedPolygons(const MultiPolygon &polygons);
	virtual void drawPolygons(const MultiPolygon &polygons);
	virtual void drawPath(const QPainterPath &path);
	virtual void drawEllipse(double x, double y, double rx, double ry);
	virtual void drawRect(double x, double y, double width, double height);
	virtual void drawImage(const Vec2D &point, const Image &image);
	virtual void drawSurface(const Vec2D &point, const Surface &surface);
	
	PaintEngineState *state() { return &_state; }
	
	void pushState();
	void popState();
	
private:
	PaintEngineState _state;
	QStack<PaintEngineState> _stateStack;
};

}

