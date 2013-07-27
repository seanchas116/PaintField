#pragma once

//ExportName: Painter

#include "paintengine.h"
#include "paintable.h"
#include "surface.h"
#include "brush.h"

namespace Malachite
{

/**
 * Painter is the painting context on Paintable objects.
 */
class MALACHITESHARED_EXPORT Painter
{
public:
	
	/**
	 * Constructs one with a paintable.
	 * @param paintable
	 */
	Painter(Paintable *paintable);
	
	/**
	 * Constructs an empty one
	 */
	Painter() {}
	
	virtual ~Painter();
	
	/**
	 * Begins painting on a paintable.
	 * @param paintable
	 * @return If succeeded
	 */
	bool begin(Paintable *paintable);
	
	/**
	 * Applies the current painting.
	 */
	void flush();
	
	/**
	 * Ends painting.
	 */
	void end();
	
	void setBlendMode(BlendMode mode) { state()->blendMode = mode; }
	BlendMode blendMode() const { return state()->blendMode; }
	
	void setBrush(const Brush &brush) { state()->brush = brush; }
	Brush brush() const { return state()->brush; }
	
	void setColor(const Color &color) { setBrush(Brush(color)); }
	void setPixel(const Pixel &argb) { setBrush(Brush(argb)); }
	
	void setOpacity(double opacity) { state()->opacity = opacity; }
	double opacity() const { return state()->opacity; }
	
	/**
	 * Sets a transform which affects the world.
	 * Same as setShapeTransform(transform.inverted()).
	 * @param transform
	 */
	void setWorldTransform(const QTransform &transform) { state()->shapeTransform = transform.inverted(); }
	QTransform worldTransform() const { return state()->shapeTransform.inverted(); }
	
	/**
	 * Sets a transform which affects the painted shape.
	 * Same as setWorldTransform(transform.inverted()).
	 * @param transform
	 */
	void setShapeTransform(const QTransform &transform) { state()->shapeTransform = transform; }
	QTransform shapeTransform() const { return state()->shapeTransform; }
	
	void setImageTransformType(Malachite::ImageTransformType type) { state()->imageTransformType = type; }
	Malachite::ImageTransformType imageTransformType() const { return state()->imageTransformType; }
	
	PaintEngineState *state() { Q_ASSERT(_paintEngine); return _paintEngine->state(); }
	const PaintEngineState *state() const { Q_ASSERT(_paintEngine); return _paintEngine->state(); }
	
	/**
	 * Draws polygons without applying transform.
	 * @param polygons
	 */
	void drawPreTransformedPolygons(const FixedMultiPolygon &polygons)
		{ Q_ASSERT(_paintEngine); _paintEngine->drawPreTransformedPolygons(polygons); }
	
	/**
	 * Draws polygons without applying transform.
	 * @param polygons
	 */
	void drawPreTransformedPolygons(const MultiPolygon &polygons)
		{ Q_ASSERT(_paintEngine); _paintEngine->drawPreTransformedPolygons(polygons); }
	
	/**
	 * Draws an image without applying transform.
	 * @param pos
	 * @param image
	 */
	void drawPreTransformedImage(const QPoint &pos, const Image &image)
		{ Q_ASSERT(_paintEngine); _paintEngine->drawPreTransformedImage(pos, image); }
	
	/**
	 * Draws an image without applying transform.
	 * @param pos
	 * @param image
	 * @param imageMaskRect	The rectangle region in "image", which will be painted.
	 */
	void drawPreTransformedImage(const QPoint &pos, const Image &image, const QRect &imageMaskRect)
		{ Q_ASSERT(_paintEngine); _paintEngine->drawPreTransformedImage(pos, image, imageMaskRect); }
	
	/**
	 * Draws a surface without applying transform.
	 * @param pos
	 * @param surface
	 */
	void drawPreTransformedSurface(const QPoint &pos, const Surface &surface)
		{ Q_ASSERT(_paintEngine); _paintEngine->drawPreTransformedSurface(pos, surface); }
	
	void drawPolygons(const MultiPolygon &polygons)
		{ Q_ASSERT(_paintEngine); _paintEngine->drawPolygons(polygons); }
	
	void drawPolygon(const Polygon &polygon)
		{ drawPolygons(MultiPolygon(polygon)); }
	
	void drawPath(const QPainterPath &path)
		{ Q_ASSERT(_paintEngine); _paintEngine->drawPath(path); }
	
	/**
	 * @param rect The bounding rectangle
	 */
	void drawEllipse(const QRectF &rect)
		{ drawEllipse(rect.x() + rect.width() * 0.5, rect.y() + rect.height() * 0.5, rect.width() * 0.5, rect.height() * 0.5); }
	
	/**
	 * @param x The x coordinate of the center
	 * @param y The y coordinate of the center
	 * @param rx The x-axis radius
	 * @param ry The y-axis radius
	 */
	void drawEllipse(double x, double y, double rx, double ry)
		{ Q_ASSERT(_paintEngine); _paintEngine->drawEllipse(x, y, rx, ry); }
	
	/**
	 * @param center
	 * @param rx The x-axis radius
	 * @param ry The y-axis radius
	 */
	void drawEllipse(const QPointF &center, double rx, double ry)
		{ drawEllipse(center.x(), center.y(), rx, ry); }
	
	void drawEllipse(const Vec2D &center, double rx, double ry)
		{ drawEllipse(center.x(), center.y(), rx, ry); }
	
	void drawRect(const QRectF &rect)
		{ drawRect(rect.x(), rect.y(), rect.width(), rect.height()); }
	
	void drawRect(double x, double y, double width, double height)
		{ Q_ASSERT(_paintEngine); _paintEngine->drawRect(x, y, width, height); }
	
	void drawImage(const Vec2D &pos, const Image &image)
		{ Q_ASSERT(_paintEngine); _paintEngine->drawImage(pos, image); }
	
	void drawImage(double x, double y, const Image &image)
		{ drawImage(Vec2D(x, y), image); }
	
	void drawSurface(const Vec2D &pos, const Surface &surface)
		{ Q_ASSERT(_paintEngine); _paintEngine->drawSurface(pos, surface); }
	
	void drawSurface(double x, double y, const Surface &surface)
		{ drawSurface(Vec2D(x, y), surface); }
	
	void pushState() { Q_ASSERT(_paintEngine); _paintEngine->pushState(); }
	void popState() { Q_ASSERT(_paintEngine); _paintEngine->popState(); }
	
	void translateShape(double dx, double dy) { state()->shapeTransform.translate(dx, dy); }
	void translateShape(const QPointF &d) { translateShape(d.x(), d.y()); }
	void rotateShape(double angle) { state()->shapeTransform.rotate(angle); }
	void scaleShape(double sx, double sy) { state()->shapeTransform.scale(sx, sy); }
	
protected:
	
	PaintEngine *paintEngine() { return _paintEngine.data(); }
	const PaintEngine *paintEngine() const { return _paintEngine.data(); }
	
private:
	
	Paintable *_paintable;
	QScopedPointer<PaintEngine> _paintEngine;
};

}

