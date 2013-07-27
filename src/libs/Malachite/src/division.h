#pragma once

//ExportName: Division

#include "global.h"
#include <QPoint>
#include <cmath>
#include <cstdlib>

namespace Malachite
{

// 余りが必ず正になる整数除算
class MALACHITESHARED_EXPORT IntDivision
{
public:
	IntDivision(int numerator, int denominator) {
		result = div(numerator, denominator);
		if (result.rem < 0) {
			result.rem += denominator;
			result.quot--;
		}
	}
	
	int quot() const { return result.quot; }
	int rem() const { return result.rem; }
	
	static void dividePoint(const QPoint &point, int divisor, QPoint *quot, QPoint *rem)
	{
		IntDivision divisionX(point.x(), divisor);
		IntDivision divisionY(point.y(), divisor);
		
		quot->rx() = divisionX.quot();
		quot->ry() = divisionY.quot();
		rem->rx() = divisionX.rem();
		rem->ry() = divisionY.rem();
	}
	
	static void dividePoint(const QPoint &point, int divisor, QPoint *quot)
	{
		IntDivision divisionX(point.x(), divisor);
		IntDivision divisionY(point.y(), divisor);
		
		quot->rx() = divisionX.quot();
		quot->ry() = divisionY.quot();
	}
	
private:
	div_t result;
};

class MALACHITESHARED_EXPORT Division
{
public:
	Division(double numerator, double denominator)
	{
		_quot = floor(numerator / denominator);
		_rem = fmod(numerator, denominator);
		
		if (_rem < 0)
		{
			_rem += denominator;
			_quot -= 1;
		}
	}
	
	double quot() const { return _quot; }
	double rem() const { return _rem; }
	
private:
	double _quot, _rem;
};

inline double align(double x, double unit)
{
	return ::round(x / unit) * unit;
}


}
