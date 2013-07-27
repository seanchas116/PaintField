#include "imagepaintengine.h"
#include "filler.h"
#include "gradientgenerator.h"
#include "scalinggenerator.h"
#include "../painter.h"

namespace Malachite
{

template <class Rasterizer, class Filler>
void fill(Rasterizer *ras, Bitmap<Pixel> *bitmap, BlendOp *blendOp, Filler *filler, float opacity)
{
	agg::scanline_pf sl;
	ImageBaseRenderer<Filler> baseRen(*bitmap, blendOp, opacity, filler);
	Renderer<ImageBaseRenderer<Filler> > ren(baseRen);
	
	renderScanlines(*ras, sl, ren);
}

template <class Rasterizer, Malachite::SpreadType SpreadType, class Source>
void drawTransformedImageBrush(Rasterizer *ras, Bitmap<Pixel> *bitmap, BlendOp *blendOp, const Source &source, float opacity, const QTransform &worldTransform, Malachite::ImageTransformType transformType)
{
	switch (transformType)
	{
	case Malachite::ImageTransformTypeNearestNeighbor:
	{
		typedef ScalingGeneratorNearestNeighbor<Source, SpreadType> Generator;
		Generator gen(&source);
		Filler<Generator, true> filler(&gen, worldTransform);
		fill(ras, bitmap, blendOp, &filler, opacity);
		return;
	}
	case Malachite::ImageTransformTypeBilinear:
	{
		typedef ScalingGeneratorBilinear<Source, SpreadType> Generator;
		Generator gen(&source);
		Filler<Generator, true> filler(&gen, worldTransform);
		fill(ras, bitmap, blendOp, &filler, opacity);
		return;
	}
	case Malachite::ImageTransformTypeBicubic:
	{
		typedef ScalingGenerator2<Source, SpreadType, ScalingWeightMethodBicubic> Generator;
		Generator gen(&source);
		Filler<Generator, true> filler(&gen, worldTransform);
		fill(ras, bitmap, blendOp, &filler, opacity);
		return;
	}
	case Malachite::ImageTransformTypeLanczos2:
	{
		typedef ScalingGenerator2<Source, SpreadType, ScalingWeightMethodLanczos2> Generator;
		Generator gen(&source);
		Filler<Generator, true> filler(&gen, worldTransform);
		fill(ras, bitmap, blendOp, &filler, opacity);
		return;
	}
	case Malachite::ImageTransformTypeLanczos2Hypot:
	{
		typedef ScalingGenerator2<Source, SpreadType, ScalingWeightMethodLanczos2Hypot> Generator;
		Generator gen(&source);
		Filler<Generator, true> filler(&gen, worldTransform);
		fill(ras, bitmap, blendOp, &filler, opacity);
		return;
	}
	default:
		return;
	}
}

template <class T_Rasterizer, Malachite::SpreadType T_SpreadType>
void drawWithSpreadType(T_Rasterizer *ras, Bitmap<Pixel> *bitmap, BlendOp *blendOp, const PaintEngineState &state)
{
	const Brush brush = state.brush;
	const float opacity = state.opacity;
	
	if (brush.type() == Malachite::BrushTypeColor)
	{
		ColorFiller filler(brush.pixel());
		fill(ras, bitmap, blendOp, &filler, opacity);
		return;
	}
	
	QTransform fillShapeTransform = brush.transform() * state.shapeTransform;
	
	if (brush.type() == Malachite::BrushTypeImage)
	{
		if (transformIsIntegerTranslating(fillShapeTransform))
		{
			QPoint offset(fillShapeTransform.dx(), fillShapeTransform.dy());
			
			ImageFiller<T_SpreadType> filler(brush.image().constBitmap(), offset);
			fill(ras, bitmap, blendOp, &filler, opacity);
			return;
		}
		else
		{
			drawTransformedImageBrush<T_Rasterizer, T_SpreadType, Bitmap<Pixel> >(ras, bitmap, blendOp, brush.image().constBitmap(), opacity, fillShapeTransform.inverted(), state.imageTransformType);
			return;
		}
	}
	if (brush.type() == Malachite::BrushTypeSurface)
	{
		drawTransformedImageBrush<T_Rasterizer, T_SpreadType, Surface>(ras, bitmap, blendOp, brush.surface(), opacity, fillShapeTransform.inverted(), state.imageTransformType);
		return;
	}
	if (brush.type() == Malachite::BrushTypeLinearGradient)
	{
		LinearGradientShape info = brush.linearGradientShape();
		
		if (info.transformable(fillShapeTransform))
		{
			info.transform(fillShapeTransform);
			fillShapeTransform = QTransform();
		}
		
		if (fillShapeTransform.isIdentity())
		{
			LinearGradientMethod method(info.start, info.end);
			GradientGenerator<ColorGradient, LinearGradientMethod, T_SpreadType> gen(brush.gradient(), &method);
			Filler<GradientGenerator<ColorGradient, LinearGradientMethod, T_SpreadType>, false> filler(&gen);
			fill(ras, bitmap, blendOp, &filler, opacity);
			return;
		}
		else
		{
			LinearGradientMethod method(info.start, info.end);
			GradientGenerator<ColorGradient, LinearGradientMethod, T_SpreadType> gen(brush.gradient(), &method);
			Filler<GradientGenerator<ColorGradient, LinearGradientMethod, T_SpreadType>, true> filler(&gen, fillShapeTransform.inverted());
			fill(ras, bitmap, blendOp, &filler, opacity);
			return;
		}
	}
	if (brush.type() == Malachite::BrushTypeRadialGradient)
	{
		RadialGradientShape info = brush.radialGradientShape();
		
		if (info.transformable(fillShapeTransform))
		{
			info.transform(fillShapeTransform);
			fillShapeTransform = QTransform();
		}
		
		if (info.center == info.focal)
		{
			if (fillShapeTransform.isIdentity())
			{
				RadialGradientMethod method(info.center, info.radius);
				GradientGenerator<ColorGradient, RadialGradientMethod, T_SpreadType> gen(brush.gradient(), &method);
				Filler<GradientGenerator<ColorGradient, RadialGradientMethod, T_SpreadType>, false> filler(&gen);
				fill(ras, bitmap, blendOp, &filler, opacity);
				return;
			}
			else
			{
				RadialGradientMethod method(info.center, info.radius);
				GradientGenerator<ColorGradient, RadialGradientMethod, T_SpreadType> gen(brush.gradient(), &method);
				Filler<GradientGenerator<ColorGradient, RadialGradientMethod, T_SpreadType>, true> filler(&gen, fillShapeTransform.inverted());
				fill(ras, bitmap, blendOp, &filler, opacity);
				return;
			}
		}
		else
		{
			if (fillShapeTransform.isIdentity())
			{
				FocalGradientMethod method(info.center, info.radius, info.focal);
				GradientGenerator<ColorGradient, FocalGradientMethod, T_SpreadType> gen(brush.gradient(), &method);
				Filler<GradientGenerator<ColorGradient, FocalGradientMethod, T_SpreadType>, false> filler(&gen);
				fill(ras, bitmap, blendOp, &filler, opacity);
				return;
			}
			else
			{
				FocalGradientMethod method(info.center, info.radius, info.focal);
				GradientGenerator<ColorGradient, FocalGradientMethod, T_SpreadType> gen(brush.gradient(), &method);
				Filler<GradientGenerator<ColorGradient, FocalGradientMethod, T_SpreadType>, true> filler(&gen, fillShapeTransform.inverted());
				fill(ras, bitmap, blendOp, &filler, opacity);
				return;
			}
		}
	}
}




ImagePaintEngine::ImagePaintEngine() :
	PaintEngine(),
	_image(0)
{}

bool ImagePaintEngine::begin(Paintable *paintable)
{
	_image = dynamic_cast<Image *>(paintable);
	if (!_image)
		return false;
	
	_bitmap = _image->bitmap();
	
	return true;
}

bool ImagePaintEngine::flush()
{
	return true;
}

void ImagePaintEngine::drawPreTransformedPolygons(const FixedMultiPolygon &polygons)
{
	agg::rasterizer_scanline_aa<> ras;
	
	for (const FixedPolygon &polygon : polygons)
	{
		if (polygon.size() < 3)
			continue;
		
		auto i = polygon.begin();
		ras.move_to(i->x, i->y);
		++i;
		
		for (; i != polygon.end(); ++i)
			ras.line_to(i->x, i->y);
	}
	
	BlendOp *op = BlendMode(state()->blendMode).op();
	
	switch (state()->brush.spreadType())
	{
		case Malachite::SpreadTypePad:
			drawWithSpreadType<agg::rasterizer_scanline_aa<>, Malachite::SpreadTypePad>(&ras, &_bitmap, op, *state());
			return;
		case Malachite::SpreadTypeRepeat:
			drawWithSpreadType<agg::rasterizer_scanline_aa<>, Malachite::SpreadTypeRepeat>(&ras, &_bitmap, op, *state());
			return;
		case Malachite::SpreadTypeReflective:
			drawWithSpreadType<agg::rasterizer_scanline_aa<>, Malachite::SpreadTypeReflective>(&ras, &_bitmap, op, *state());
			return;
		default:
			return;
	}
}

void ImagePaintEngine::drawPreTransformedImage(const QPoint &point, const Image &image, const QRect &imageMaskRect)
{
	QRect dstRect = _image->rect();
	QRect srcRect = (image.rect() & imageMaskRect).translated(point);
	
	QRect targetRect = dstRect & srcRect;
	
	if (targetRect.isEmpty())
		return;
	
	BlendOp *op = BlendMode(state()->blendMode).op();
	if (!op)
		return;
	
	for (int y = targetRect.top(); y <= targetRect.bottom(); ++y)
	{
		QPoint p(targetRect.left(), y);
		
		if (state()->opacity == 1.0)
			op->blend(targetRect.width(), _bitmap.pixelPointer(p), image.constPixelPointer(p - point));
		else
			op->blend(targetRect.width(), _bitmap.pixelPointer(p), image.constPixelPointer(p - point), state()->opacity);
	}
}

}

