#pragma once

//ExportName: Bitmap

#include "misc.h"
#include "pixeliterator.h"
#include <QSize>
#include <QRect>

namespace Malachite
{

template <class TDerived>
class ImageSizeAccessible
{
	const TDerived *derived() const { return static_cast<const TDerived *>(this); }
	QSize size() const { return derived()->size(); }
public:
	int width() const { return size().width(); }
	int height() const { return size().height(); }
	QRect rect() const { return QRect(QPoint(), size()); }
	int area() const { return size().height() * size().width(); }
};

template <class TDerived, class TIterator, class TConstIterator>
class ImagePixelsAccessible
{
	using value_type = typename std::iterator_traits<TIterator>::value_type;
	using iterator = TIterator;
	using const_iterator = TConstIterator;

	const TDerived *derived() const { return static_cast<const TDerived *>(this); }
	TDerived *derived() { return static_cast<TDerived *>(this); }

	iterator scanline(int y) { return derived()->scanline(y); }
	const_iterator constScanline(int y) const { return derived()->constScanline(y); }
	QSize size() const { return derived()->size(); }

public:

	iterator pixelPointer(int x, int y)
	{
		Q_ASSERT(0 <= x && x < size().width());
		return scanline(y) + x;
	}
	iterator pixelPointer(const QPoint &p) { return pixelPointer(p.x(), p.y()); }

	const_iterator constPixelPointer(int x, int y) const
	{
		Q_ASSERT(0 <= x && x < size().width());
		return constScanline(y) + x;
	}
	const_iterator constPixelPointer(const QPoint &p) const { return constPixelPointer(p.x(), p.y()); }

	value_type pixel(int x, int y) const { return *constPixelPointer(x, y); }
	value_type pixel(const QPoint &p) const { return pixel(p.x(), p.y()); }

	void setPixel(int x, int y, const value_type &color)
	{
		*pixelPointer(x, y) = color;
	}

	void setPixel(const QPoint &p, const value_type &color) { setPixel(p.x(), p.y(), color); }

	void fill(const value_type &c)
	{
		auto s = size();
		for (int y = 0; y < s.height(); ++y) {
			auto p = scanline(y);
			std::fill(p, p + s.width(), c);
		}
	}

	template <class TSrcImage>
	void paste(const TSrcImage &image, const QPoint &point = QPoint())
	{
		QRect r = QRect(QPoint(), size()) & QRect(point, image.size());

		for (int y = r.top(); y <= r.bottom(); ++y) {
			QPoint pos(r.left(), y);
			auto dp = pixelPointer(pos);
			auto sp = image.constPixelPointer(pos - point);
			std::copy(sp, sp + r.width(), dp);
		}
	}
};

template <class TImage>
class InvertedImage :
	public ImageSizeAccessible<InvertedImage<TImage>>,
	public ImagePixelsAccessible<
		InvertedImage<TImage>,
		typename TImage::iterator,
		typename TImage::const_iterator>
{
public:

	using value_type = typename TImage::value_type;
	using iterator = typename TImage::iterator;
	using const_iterator = typename TImage::const_iterator;

	InvertedImage(const TImage &image) :
		mBase(image)
	{}

	iterator scanline(int y)
	{
		return mBase.scanline(size().height() - y - 1);
	}

	const_iterator constScanline(int y) const
	{
		return mBase.constScanline(size().height() - y - 1);
	}

	QSize size() const { return mBase.size(); }
	iterator begin() { return mBase.begin(); }
	const_iterator cbegin() const { return mBase.cbegin(); }

private:

	TImage mBase;
};

template <class TImage>
inline InvertedImage<TImage> invertImage(const TImage &image)
{
	return InvertedImage<TImage>(image);
}

template <typename TPixel>
class MALACHITESHARED_EXPORT Bitmap :
	public ImageSizeAccessible<Bitmap<TPixel>>,
	public ImagePixelsAccessible<Bitmap<TPixel>, PixelIterator<TPixel>, ConstPixelIterator<TPixel>>
{
public:
	
	using PixelType = TPixel;
	using value_type = TPixel;
	using iterator = PixelIterator<TPixel>;
	using const_iterator = ConstPixelIterator<TPixel>;
	
	Bitmap() = default;
	
	Bitmap(PixelIterator<PixelType> bits, const QSize &size) :
		_bits(bits),
		_size(size)
	{}

	operator Bitmap<const TPixel>() const
	{
		return Bitmap<const TPixel>(_bits, _size);
	}

	iterator scanline(int y)
	{
		Q_ASSERT(0 <= y && y < size().height());
		return begin() + size().width() * y;
	}

	const_iterator constScanline(int y) const
	{
		Q_ASSERT(0 <= y && y < size().height());
		return begin() + size().width() * y;
	}
	
	iterator begin() { return _bits; }
	iterator end() { return _bits + this->area(); }
	const_iterator cbegin() const { return _bits; }
	const_iterator cend() const { return _bits + this->area(); }
	const_iterator begin() const { return cbegin(); }
	const_iterator end() const { return cend(); }

	QSize size() const { return _size; }
	
private:
	
	PixelIterator<PixelType> _bits;
	QSize _size;
};

}

