#include "colorgradient.h"

namespace Malachite
{

ColorGradientCache::ColorGradientCache(ColorGradient *gradient, int sampleCount) :
	ColorGradient(),
	_sampleCount(sampleCount),
	_cache(sampleCount + 1)
{
	for (int i = 0; i <= sampleCount; ++i)
	{
		_cache[i] = gradient->at(1.0 / sampleCount * i);
	}
}


Pixel ArgbGradient::at(float x) const
{
	int count = _stops.size();
	if (count == 0)
		return Pixel(0);
	if (count == 1)
		return _stops.value(0);
	
	QMapIterator<float, Pixel> i(_stops);
	
	if (x <= i.peekNext().key()) return i.peekNext().value();
	i.next();
	
	for (; i.hasNext(); i.next())
	{
		float x1 = i.peekNext().key();
		if (x < x1)
		{
			float x0 = i.peekPrevious().key();
			Pixel y0 = i.peekPrevious().value();
			Pixel y1 = i.peekNext().value();
			
			Pixel r;
			r.rv() = y0.v() + (x - x0) / (x1 - x0) * (y1.v() - y0.v());
			return r;
		}
		if (x == x1)
			return i.peekNext().value();
	}
	
	return _stops.values().last();
}

}
