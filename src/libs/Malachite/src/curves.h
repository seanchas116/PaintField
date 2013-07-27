#pragma once

//ExportName: Curves

#include "polygon.h"
#include "global.h"

namespace Malachite
{

class MALACHITESHARED_EXPORT Curve4
{
public:
	Curve4() {}
	Curve4(const Vec2D &start, const Vec2D &control1, const Vec2D &control2, const Vec2D &end)
	{
		this->start = start;
		this->control1 = control1;
		this->control2 = control2;
		this->end = end;
	}
	
	static Curve4 fromCatmullRom(const Vec2D &previous, const Vec2D &start, const Vec2D &end, const Vec2D &next)
	{
		return Curve4(start, start + (end - previous) / 6.0, end - (next - start) / 6.0, end);
	}
	
	static Curve4 fromCatmullRomStart(const Vec2D &start, const Vec2D &end, const Vec2D &next)
	{
		return Curve4(start, start + (end - start) / 3.0, end - (next - start) / 6.0, end);
	}
	
	static Curve4 fromCatmullRomEnd(const Vec2D &previous, const Vec2D &start, const Vec2D &end)
	{
		return Curve4(start, start + (end - previous) / 6.0, (end - start) / 3.0, end);
	}
	
	static Curve4 fromCatmullRom(const Polygon &polygon, int indexStart);
	
	static Curve4 fromBSpline(const Vec2D &c0, const Vec2D &c1, const Vec2D &c2, const Vec2D &c3)
	{
		return Curve4((c0 + 4.0*c1 + c2)/6.0,
		              (2.0*c1 + c2)/3.0,
		              (c1 + 2.0*c2)/3.0,
		              (c1 + 4.0*c2 + c3)/6.0);
	}
	
	Vec2D start, control1, control2, end;
};

}

