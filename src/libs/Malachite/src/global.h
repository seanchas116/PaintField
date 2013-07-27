#pragma once

//ExportName: Global

#include <Qt>
#include <QEvent>
#include <QMetaType>

#if defined(MALACHITE_LIBRARY)
#  define MALACHITESHARED_EXPORT Q_DECL_EXPORT
#else
#  define MALACHITESHARED_EXPORT Q_DECL_IMPORT
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Malachite
{

enum ImageFormat
{
	ImageFormatArgb,
	ImageFormatArgbFast,
	ImageFormatRgb,
	ImageFormatBgra
};

enum ImageComponentType
{
	ImageComponentTypeFloat,
	ImageComponentTypeUint16,
	ImageComponentTypeUint8
};

enum BrushType
{
	BrushTypeNull,
	BrushTypeColor,
	BrushTypeLinearGradient,
	BrushTypeRadialGradient,
	BrushTypeImage,
	BrushTypeSurface
};

enum SpreadType
{
	SpreadTypeWindowed,
	SpreadTypePad,
	SpreadTypeRepeat,
	SpreadTypeReflective
};

enum GradientType
{
	GradientTypeNull,
	GradientTypeLinear,
	GradientTypeRadial
};

enum ImageTransformType
{
	ImageTransformTypeNearestNeighbor,
	ImageTransformTypeBilinear,
	ImageTransformTypeBicubic,
	ImageTransformTypeLanczos2,
	ImageTransformTypeLanczos2Hypot
};

enum PixelFieldType
{
	PixelFieldImage,
	PixelFieldSurface
};

}

