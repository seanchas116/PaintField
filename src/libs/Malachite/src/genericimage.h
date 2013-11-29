#pragma once

//ExportName: GenericImage

#include "bitmap.h"
#include "pixelarray.h"
#include <QRect>
#include <QSharedDataPointer>
#include <boost/operators.hpp>


namespace Malachite
{

template <typename TPixel>
class GenericImageData : public QSharedData
{
public:
	
	typedef TPixel PixelType;
	
	GenericImageData(const QSize &size)
	{
		initWithNewData(size);
	}
	
	GenericImageData(PixelType *bits, const QSize &size) :
		mBitmap(makePixelIterator(bits, bits + size.width() * size.height()), size)
	{
	}
	
	GenericImageData(const GenericImageData &other) :
		QSharedData(other)
	{
		initWithNewData(other.mBitmap.size());
		std::copy(other.mBitmap.begin(), other.mBitmap.end(), mBitmap.begin());
	}

	void initWithNewData(const QSize &size)
	{
		auto area = size.width() * size.height();
		mData = PixelArray<PixelType>(area);
		mBitmap = Bitmap<PixelType>(mData.begin(), size);
	}

	PixelArray<PixelType> mData;
	Bitmap<PixelType> mBitmap;
};

template <class TPixel>
class MALACHITESHARED_EXPORT GenericImage :
	public ImageSizeAccessible<GenericImage<TPixel>>,
	public ImagePixelsAccessible<GenericImage<TPixel>, PixelIterator<TPixel>, ConstPixelIterator<TPixel>>,
	boost::equality_comparable<GenericImage<TPixel>>
{
public:
	
	using Self = GenericImage<TPixel>;
	using Data = GenericImageData<TPixel>;
	using PixelType = TPixel;
	using value_type = TPixel;
	using iterator = PixelIterator<value_type>;
	using const_iterator = ConstPixelIterator<value_type>;
	
	GenericImage() = default;
	
	GenericImage(const QSize &size)
	{
		if (!size.isEmpty())
			p = new Data(size);
	}
	
	GenericImage(int width, int height) : GenericImage(QSize(width, height)) {}
	
	/**
	 * Wraps existing data.
	 * The GenericImage will not take ownership of data.
	 * @param data
	 * @param size
	 * @return 
	 */
	static GenericImage wrap(value_type *data, const QSize &size)
	{
		GenericImage r;
		if (!size.isEmpty())
			r.p = new Data(data, size);
		return r;
	}
	
	void detach() { p.detach(); }
	bool isValid() const { return p; }
	
	QSize size() const { return p ? p->mBitmap.size() : QSize(); }

	Bitmap<value_type> bitmap() { return p ? p->mBitmap : Bitmap<value_type>(); }
	Bitmap<const value_type> constBitmap() const { return p ? p->mBitmap : Bitmap<const value_type>(); }

	iterator scanline(int y) { Q_ASSERT(p); return p->mBitmap.scanline(y); }
	const_iterator constScanline(int y) const { Q_ASSERT(p); return p->mBitmap.constScanline(y); }
	iterator begin() { Q_ASSERT(p); return p->mBitmap.begin(); }
	iterator end() { Q_ASSERT(p); return p->mBitmap.end(); }
	const_iterator cbegin() const { Q_ASSERT(p); return p->mBitmap.cbegin(); }
	const_iterator cend() const { Q_ASSERT(p); return p->mBitmap.cend(); }
	const_iterator begin() const { return cbegin(); }
	const_iterator end() const { return cend(); }

	void clear()
	{
		this->fill(0);
	}
	
	template <class NewPixel>
	GenericImage<NewPixel> convert() const
	{
		if (!p)
			return GenericImage<NewPixel>();
		
		GenericImage<NewPixel> newImage(size());
		std::copy(this->cbegin(), this->cend(), newImage.begin());
		return newImage;
	}
	
	bool operator==(const GenericImage &other) const
	{
		if (this->p == other.p)
			return true;
		
		if (this->isValid() != other.isValid())
			return false;
		
		if (this->size() != other.size())
			return false;

		return std::equal(this->begin(), this->end(), other.begin());
	}
	
	bool referenceIsEqualTo(const GenericImage &other) const
	{
		return p == other.p;
	}
	
private:
	QSharedDataPointer<Data> p;
};

}

