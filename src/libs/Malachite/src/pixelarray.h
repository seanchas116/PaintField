#pragma once

//ExportName: PixelArray

#include "pixeliterator.h"
#include <memory>

namespace Malachite
{

template <class TPixel>
class PixelArray
{
public:

	PixelArray() = default;

	PixelArray(int size) :
		mData(new TPixel[size]),
		mSize(size)
	{}

	PixelIterator<TPixel> begin()
	{
		return makePixelIterator(mData.get(), mData.get() + mSize);
	}

	ConstPixelIterator<TPixel> cbegin() const
	{
		return makePixelIterator(mData.get(), mData.get() + mSize);
	}

	ConstPixelIterator<TPixel> begin() const { return cbegin(); }

	PixelIterator<TPixel> end() { return begin() + mSize; }
	PixelIterator<TPixel> cend() const { return cbegin() + mSize; }
	PixelIterator<TPixel> end() const { return cend(); }

	int size() const { return mSize; }

private:

	std::unique_ptr<TPixel[]> mData;
	int mSize = 0;
};

}
