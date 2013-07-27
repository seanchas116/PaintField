#pragma once

//ExportName: CurveSubdivision

#include "polygon.h"
#include "curves.h"

namespace Malachite
{

class MALACHITESHARED_EXPORT CurveSubdivision
{
public:
	
	enum Type
	{
		/**
		 * Incremental, normal curve subdivision way
		 */
		TypeIncremental,
		/**
		 * Adaptive, see http://antigrain.com/research/adaptive_bezier/
		 */
		TypeAdaptive
	};
	
	CurveSubdivision(const Curve4 &curve, Type type = TypeAdaptive);
	CurveSubdivision(const Vec2D &start, const Vec2D &control1, const Vec2D &control2, const Vec2D &end, Type type = TypeAdaptive);
	
	Polygon polygon() const { return _polygon; }
	
private:
	Polygon _polygon;
};

}

