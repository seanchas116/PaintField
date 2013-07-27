#pragma once

//ExportName: Interval

#include <algorithm>
#include <array>
#include "global.h"

namespace Malachite {

class MALACHITESHARED_EXPORT Interval
{
public:
	
	Interval() : _start(0), _end(0) {}
	Interval(int start, int length) : _start(start), _end(start + length) {}
	
	int start() const { return _start; }
	int length() const { return _end - _start; }
	int end() const { return _end; }
	
	bool isValid() const { return _end > _start; }
	
	void setStart(int start) { _start = start; }
	void setEnd(int end) { _end = end; }
	void setLength(int len) { _end = _start + len; }
	
	void shift(int diff)
	{
		*this = shifted(diff);
	}
	
	Interval shifted(int diff) const
	{
		Interval result;
		result.setStart(start() + diff);
		result.setEnd(end() + diff);
		return result;
	}
	
	void moveStart(int start) { shift(start - _start); }
	void moveEnd(int end) { shift(end - _end); }
	
	std::array<Interval, 2> subtracted(const Interval &other) const
	{
		return subtract(*this, other);
	}
	
	static std::array<Interval, 2> subtract(const Interval &i1, const Interval &i2)
	{
		Interval left, right;
		left.setStart(i1.start());
		left.setEnd(std::min(i2.start(), i1.end()));
		right.setStart(std::max(i2.end(), i1.start()));
		right.setEnd(i1.end());
		return {{left, right}};
	}
	
private:
	
	int _start, _end;
};

inline Interval operator|(const Interval &i1, const Interval &i2)
{
	Interval result;
	result.setStart(std::min(i1.start(), i2.start()));
	result.setEnd(std::max(i1.end(), i2.end()));
	return result;
}

inline Interval operator&(const Interval &i1, const Interval &i2)
{
	Interval result;
	result.setStart(std::max(i1.start(), i2.start()));
	result.setEnd(std::min(i1.end(), i2.end()));
	return result;
}

}
