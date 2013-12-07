#pragma once

//ExportName: PixelIterator

#include <Qt>
#include <boost/iterator/iterator_adaptor.hpp>

#ifdef QT_DEBUG
#define ML_DEBUG_MEMORY
#endif

namespace Malachite
{

template <class TPixel>
class AssertedPixelIterator :  public boost::iterator_adaptor<AssertedPixelIterator<TPixel>, TPixel *>
{
	using base_type = boost::iterator_adaptor<AssertedPixelIterator<TPixel>, TPixel *>;
public:

	AssertedPixelIterator() = default;

	AssertedPixelIterator(TPixel *current, TPixel *end) :
		base_type(current),
		mInitialized(true),
		mEnd(end)
	{}

	operator AssertedPixelIterator<const TPixel>() const
	{
		return AssertedPixelIterator<const TPixel>(this->base(), mEnd);
	}

	explicit operator TPixel *() const
	{
		return this->base();
	}

private:

	friend class boost::iterator_core_access;

	TPixel &dereference() const
	{
		Q_ASSERT(isValid());
		return *this->base();
	}

	bool isValid() const
	{
		return mInitialized && this->base() < mEnd;
	}

	bool mInitialized = false;
	TPixel *mEnd;
};

#ifdef ML_DEBUG_MEMORY
template <class TPixel>
using PixelIterator = AssertedPixelIterator<TPixel>;
#else
template <class TPixel>
using PixelIterator = TPixel *;
#endif

template <class TPixel>
using ConstPixelIterator = PixelIterator<const TPixel>;

template <class TPixel>
PixelIterator<TPixel> makePixelIterator(TPixel *p, TPixel *end)
{
#ifdef ML_DEBUG_MEMORY
	return AssertedPixelIterator<TPixel>(p, end);
#else
	Q_UNUSED(end);
	return p;
#endif
}

template <class TPixel>
PixelIterator<TPixel> makePixelIterator(TPixel *p, int size)
{
	return makePixelIterator(p, p + size);
}


}
