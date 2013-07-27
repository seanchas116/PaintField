#pragma once

//ExportName: GenericImage

#include <QRect>
#include <QSharedDataPointer>
#include "bitmap.h"

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

enum ImagePasteInversionMode
{
	ImagePasteNotInverted = 0,
	ImagePasteDestinationInverted = 1,
	ImagePasteSourceInverted = 2,
	ImagePasteBothInverted = ImagePasteDestinationInverted | ImagePasteSourceInverted
};

template <class T_Pixel>
class MALACHITESHARED_EXPORT GenericImage
{
public:
	
	/**
	 * Pixel type
	 */
	typedef T_Pixel PixelType;
	
	typedef GenericImage<PixelType> SelfType;
	
	/**
	 * Constructs an empty image.
	 */
	GenericImage() {}
	
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
	
	QSize size() const
		{ return p ? p->bitmap.size() : QSize(); }
	QRect rect() const
		{ return QRect(QPoint(), size()); }
	int width() const
		{ return p ? p->bitmap.width() : 0; }
	int height() const
		{ return p ? p->bitmap.height() : 0; }
	int bytesPerLine() const
		{ return p ? p->bitmap.bytesPerLine() : 0; }
	int area() const
		{ return p ? p->bitmap.area() : 0; }
	
	Bitmap<PixelType> bitmap() { return p ? p->bitmap : Bitmap<PixelType>(); }
	const Bitmap<PixelType> constBitmap() const { return p ? p->bitmap : Bitmap<PixelType>(); }
	
	Pointer<PixelType> bits()
		{ Q_ASSERT(p); return p->bitmap.bits(); }
	Pointer<const PixelType> constBits() const
		{ Q_ASSERT(p); return p->bitmap.constBits(); }
	
	Pointer<PixelType> scanline(int y)
		{ Q_ASSERT(p); return p->bitmap.scanline(y); }
	Pointer<const PixelType> constScanline(int y) const
		{ Q_ASSERT(p); return p->bitmap.constScanline(y); }
	
	Pointer<PixelType> invertedScanline(int invertedY)
		{ Q_ASSERT(p); return p->bitmap.invertedScanline(invertedY); }
	Pointer<const PixelType> invertedConstScanline(int invertedY) const
		{ Q_ASSERT(p); return p->bitmap.invertedConstScanline(invertedY); }
	
	Pointer<PixelType> pixelPointer(int x, int y)
		{ Q_ASSERT(p); return p->bitmap.pixelPointer(x, y); }
	Pointer<PixelType> pixelPointer(const QPoint &point)
		{ return pixelPointer(point.x(), point.y()); }
	
	Pointer<const PixelType> constPixelPointer(int x, int y) const
		{ Q_ASSERT(p); return p->bitmap.constPixelPointer(x, y); }
	Pointer<const PixelType> constPixelPointer(const QPoint &point) const
		{ return constPixelPointer(point.x(), point.y()); }
	
	PixelType pixel(int x, int y) const { return *constPixelPointer(x, y); }
	PixelType pixel(const QPoint &point) const { return pixel(point.x(), point.y()); }
	
	void setPixel(int x, int y, const PixelType &color)
	{
		auto p = pixelPointer(x, y);
		Q_ASSERT(p);
		*p = color;
	}
	
	void setPixel(const QPoint &p, const PixelType &color) { setPixel(p.x(), p.y(), color); }
	
	void fill(const PixelType &c)
	{
		Q_ASSERT(p);
		
		QSize s = size();
		for (int y = 0; y < s.height(); ++y)
		{
			scanline(y).fill(c, s.width());
		}
	}
	
	void clear()
	{
		fill(0);
	}
	
	template <class NewPixel>
	GenericImage<NewPixel> convert()
	{
		if (!p)
			return GenericImage<NewPixel>();
		
		QSize s = size();
		GenericImage<NewPixel> newImage(s);
		
		for (int y = 0; y < s.height(); ++y)
		{
			NewPixel *dp = newImage.scanline(y);
			PixelType *sp = constScanline(y);
			
			for (int x = 0; x < s.width(); ++x)
				*dp++ = *sp++;
		}
	}
	
	template <ImagePasteInversionMode InversionMode = ImagePasteNotInverted, class SrcImage>
	void paste(const SrcImage &image, const QPoint &point = QPoint())
	{
		QRect r = rect() & QRect(point, image.size());
		
		for (int y = r.top(); y <= r.bottom(); ++y)
		{
			PixelType *dp;
			
			if (InversionMode & ImagePasteDestinationInverted)
				dp = invertedScanline(y);
			else
				dp = scanline(y);
			
			dp += r.left();
			
			const typename SrcImage::PixelType *sp;
			
			if (InversionMode & ImagePasteSourceInverted)
				sp = image.invertedConstScanline(y - point.y());
			else
				sp = image.constScanline(y - point.y());
			
			sp += (r.left() - point.x());
			
			for (int x = 0; x < r.width(); ++x)
				*dp++ = *sp++;
		}
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
	
	bool operator!=(const GenericImage &other) const
	{
		return !(*this == other);
	}
	
	bool referenceIsEqualTo(const GenericImage &other) const
	{
		return p == other.p;
	}
	
private:
	QSharedDataPointer<GenericImageData<PixelType> > p;
};

}

