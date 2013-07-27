#pragma once

//ExportName: Brush

#include <QVariant>
#include <QSharedDataPointer>
#include "color.h"
#include "colorgradient.h"
#include "image.h"
#include "surface.h"
#include <QTransform>
#include <QScopedPointer>

namespace Malachite
{

class BrushData : public QSharedData
{
public:
	BrushData() :
		type(Malachite::BrushTypeNull),
		spreadType(Malachite::SpreadTypeRepeat)
	{}
	
	BrushData(const Color &color) :
		type(Malachite::BrushTypeColor),
		spreadType(Malachite::SpreadTypeRepeat),
		data(QVariant::fromValue(color))
	{}
	
	BrushData(const Image &image) :
		type(Malachite::BrushTypeImage),
		spreadType(Malachite::SpreadTypeRepeat),
		data(QVariant::fromValue(image))
	{}
	
	BrushData(const ColorGradient &gradient, const LinearGradientShape &shape) :
		type(Malachite::BrushTypeLinearGradient),
		spreadType(Malachite::SpreadTypePad),
		data(QVariant::fromValue(shape)),
		gradient(gradient.clone())
	{}
	
	BrushData(const ColorGradient &gradient, const RadialGradientShape &shape) :
		type(Malachite::BrushTypeRadialGradient),
		spreadType(Malachite::SpreadTypePad),
		data(QVariant::fromValue(shape)),
		gradient(gradient.clone())
	{}
	
	BrushData(const Surface &surface) :
		type(Malachite::BrushTypeSurface),
		spreadType(Malachite::SpreadTypeRepeat),
		data(QVariant::fromValue(surface))
	{}
	
	BrushData(const BrushData &other) :
		QSharedData(other),
		type(other.type),
		spreadType(other.spreadType),
		data(other.data),
		transform(other.transform),
		gradient(gradient->clone())
	{}
	
	Malachite::BrushType type;
	Malachite::SpreadType spreadType;
	QVariant data;
	QTransform transform;
	QScopedPointer<ColorGradient> gradient;
};


/**
 * The Brush specifies how shapes are "filled".
 */
class MALACHITESHARED_EXPORT Brush
{
public:
	
	Brush() :
		d(new BrushData())
	{}
	
	Brush(const Color &color) :
		d(new BrushData(color))
	{}
	
	Brush(const Pixel &argb) :
		d(new BrushData(Color::fromPixel(argb)))
	{}
	
	Brush(const ColorGradient &gradient, const LinearGradientShape &shape) :
		d(new BrushData(gradient, shape))
	{}
	
	Brush(const ColorGradient &gradient, const RadialGradientShape &shape) :
		d(new BrushData(gradient, shape))
	{}
	
	Brush(const Image &image) :
		d(new BrushData(image))
	{}
	
	Brush(const Surface &surface) :
		d(new BrushData(surface))
	{}
	
	static Brush fromLinearGradient(const ColorGradient &gradient, const Vec2D &start, const Vec2D &end)
	{
		return Brush(gradient, LinearGradientShape(start, end));
	}
	
	static Brush fromRadialGradient(const ColorGradient &gradient, const Vec2D &center, double radius, const Vec2D &focal)
	{
		return Brush(gradient, RadialGradientShape(center, radius, focal));
	}
	
	static Brush fromRadialGradient(const ColorGradient &gradient, const Vec2D &center, const Vec2D &radius, const Vec2D &focal)
	{
		return Brush(gradient, RadialGradientShape(center, radius, focal));
	}
	
	static Brush fromRadialGradient(const ColorGradient &gradient, const Vec2D &center, double radius)
	{
		return Brush(gradient, RadialGradientShape(center, radius));
	}
	
	static Brush fromRadialGradient(const ColorGradient &gradient, const Vec2D &center, const Vec2D &radius)
	{
		return Brush(gradient, RadialGradientShape(center, radius));
	}
	
	bool isValid() { return d; }
	
	Malachite::BrushType type() const { return d->type; }
	
	void setSpreadType(Malachite::SpreadType type) { d->spreadType = type; }
	Malachite::SpreadType spreadType() const { return d->spreadType; }
	
	Color color() const { return d->type == Malachite::BrushTypeColor ? d->data.value<Color>() : Color(); }
	Pixel pixel() const { return color().toPixel(); }
	Image image() const { return d->type == Malachite::BrushTypeImage ? d->data.value<Image>() : Image(); }
	Surface surface() const { return d->type == Malachite::BrushTypeSurface ? d->data.value<Surface>() : Surface(); }
	
	LinearGradientShape linearGradientShape() const { return d->type == Malachite::BrushTypeLinearGradient ? d->data.value<LinearGradientShape>() : LinearGradientShape(); }
	RadialGradientShape radialGradientShape() const { return d->type == Malachite::BrushTypeRadialGradient ? d->data.value<RadialGradientShape>() : RadialGradientShape(); }
	
	const ColorGradient *gradient() const { return d->gradient.data(); }
	
	void setTransform(const QTransform &transform) { d->transform = transform; }
	QTransform transform() const { return d->transform; }
	
	void translate(double dx, double dy) { d->transform.translate(dx, dy); }
	void translate(const QPointF &d) { translate(d.x(), d.y()); }
	void translate(const QPoint &d) { translate(d.x(), d.y()); }
	void scale(double sx, double sy) { d->transform.scale(sx, sy); }
	void rotate(double angle) { d->transform.rotate(angle); }
	
private:
	
	QSharedDataPointer<BrushData> d;
};

}

Q_DECLARE_METATYPE(Malachite::Brush)

