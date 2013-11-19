#pragma once

//ExportName: GenericImage

#include "bitmap.h"
#include <QRect>
#include <QSharedDataPointer>
#include <boost/operators.hpp>


namespace Malachite
{

template <typename T_Pixel>
class GenericImageData : public QSharedData
{
public:
	
	typedef T_Pixel PixelType;
	
	GenericImageData(const QSize &size, int bytesPerLine) :
		bitmap(Pointer<PixelType>(), size, bytesPerLine),
		ownsData(true)
	{
		bitmap.setBits(new PixelType[bitmap.byteCount()], bitmap.byteCount());
	}
	
	GenericImageData(void *bits, const QSize &size, int bytesPerLine) :
		bitmap(Pointer<PixelType>(), size, bytesPerLine),
		ownsData(false)
	{
		bitmap.setBits(bits, bitmap.byteCount());
	}
	
	GenericImageData(const GenericImageData &other) :
		QSharedData(other),
		bitmap(other.bitmap),
		ownsData(true)
	{
		bitmap.setBits(new PixelType[bitmap.byteCount()], bitmap.byteCount());
		bitmap.bits().pasteByte(other.bitmap.constBits(), other.bitmap.byteCount());
	}
	
	~GenericImageData()
	{
		if (ownsData)
			delete[] bitmap.bits();
	}
	
	Bitmap<PixelType> bitmap;
	const bool ownsData;
};

template <class TPixel>
class MALACHITESHARED_EXPORT GenericImage :
	public ImageSizeAccessible<GenericImage<TPixel>>,
	public BitsAccessible<GenericImage<TPixel>, TPixel>,
	private boost::equality_comparable<GenericImage<TPixel>>
{
public:
	
	/**
	 * Pixel type
	 */
	typedef TPixel PixelType;
	
	typedef GenericImage<PixelType> SelfType;
	
	/**
	 * Constructs an empty image.
	 */
	GenericImage() = default;
	
	/**
	 * Constructs an image with a size and byte count per line.
	 * @param size
	 * @param bytesPerLine
	 */
	GenericImage(const QSize &size, int bytesPerLine)
	{
		if (!size.isEmpty())
			p = new GenericImageData<PixelType>(size, bytesPerLine);
	}
	
	/**
	 * Constructs an image with a size.
	 * bytePerLine will be size.width() * sizeof(PixelType).
	 * @param size
	 */
	GenericImage(const QSize &size) : GenericImage(size, size.width() * sizeof(PixelType)) {}
	
	/**
	 * Constructs an image with a size and byte count per line.
	 * @param width
	 * @param height
	 * @param bytesPerLine
	 */
	GenericImage(int width, int height, int bytesPerLine) : GenericImage(QSize(width, height), bytesPerLine) {}
	
	/**
	 * Constructs an image with a size.
	 * bytePerLine will be width * sizeof(PixelType).
	 * @param width
	 * @param height
	 */
	GenericImage(int width, int height) : GenericImage(QSize(width, height), width * sizeof(PixelType)) {}
	
	/**
	 * Wraps existing data.
	 * The GenericImage will not take ownership of data.
	 * @param data
	 * @param size
	 * @param bytesPerLine
	 * @return 
	 */
	static GenericImage wrap(void *data, const QSize &size, int bytesPerLine)
	{
		GenericImage r;
		if (!size.isEmpty())
			r.p = new GenericImageData<PixelType>(data, size, bytesPerLine);
		return r;
	}
	
	static GenericImage wrap(void *data, const QSize &size) { return wrap(data, size, size.width() * sizeof(PixelType)); }
	
	static const GenericImage wrap(const void *data, const QSize &size, int bytesPerLine)
	{
		return wrap(const_cast<void *>(data), size, bytesPerLine);
	}
	
	static const GenericImage wrap(const void *data, const QSize &size) { return wrap(data, size, size.width() * sizeof(PixelType)); }
	
	void detach() { p.detach(); }
	bool isValid() const { return p; }
	
	QSize size() const { return p ? p->bitmap.size() : QSize(); }

	Bitmap<PixelType> bitmap() { return p ? p->bitmap : Bitmap<PixelType>(); }
	const Bitmap<PixelType> constBitmap() const { return p ? p->bitmap : Bitmap<PixelType>(); }
	int bytesPerLine() const { Q_ASSERT(p); return p->bitmap.bytesPerLine(); }
	
	Pointer<PixelType> bits()
		{ Q_ASSERT(p); return p->bitmap.bits(); }
	Pointer<const PixelType> constBits() const
		{ Q_ASSERT(p); return p->bitmap.constBits(); }

	void clear()
	{
		this->fill(0);
	}
	
	template <class NewPixel>
	GenericImage<NewPixel> convert()
	{
		if (!p)
			return GenericImage<NewPixel>();
		
		GenericImage<NewPixel> newImage(size());
		return newImage.paste(*this);
	}
	
	bool operator==(const GenericImage &other) const
	{
		if (p == other.p)
			return true;
		
		if (isValid() != other.isValid())
			return false;
		
		Q_ASSERT(p);
		
		QSize size = p->bitmap.size();
		
		if (size != other.p->bitmap.size())
			return false;
		
		for (int y = 0; y < size.height(); ++y)
		{
			if (memcmp(p->bitmap.constScanline(y), other.p->bitmap.constScanline(y), size.width() * sizeof(PixelType)))
				return false;
		}
		
		return true;
	}
	
	bool referenceIsEqualTo(const GenericImage &other) const
	{
		return p == other.p;
	}
	
private:
	QSharedDataPointer<GenericImageData<PixelType> > p;
};

}

