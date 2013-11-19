#pragma once

//ExportName: Bitmap

#include "misc.h"
#include "memory.h"
#include <QSize>
#include <QRect>

namespace Malachite
{

template <class TDerived>
struct ImageSizeAccessible
{
	int width() const { return size().width(); }
	int height() const { return size().height(); }
	QRect rect() const { return QRect(QPoint(), size()); }
	int area() const { return size().height() * size().width(); }
private:
	const TDerived *derived() const { return static_cast<const TDerived *>(this); }
	QSize size() const { return derived()->size(); }
};

enum ImagePasteInversionMode
{
	ImagePasteNotInverted = 0,
	ImagePasteDestinationInverted = 1,
	ImagePasteSourceInverted = 2,
	ImagePasteBothInverted = ImagePasteDestinationInverted | ImagePasteSourceInverted
};

template <class TDerived, class TPixel>
struct BitsAccessible
{
	int byteCount() const { return bytesPerLine() * size().height(); }

	Pointer<TPixel> scanline(int y)
	{
		Q_ASSERT(0 <= y && y < size().height());
		return bits().byteOffset(bytesPerLine() * y);
	}
	Pointer<const TPixel> constScanline(int y) const
	{
		Q_ASSERT(0 <= y && y < size().height());
		return constBits().byteOffset(bytesPerLine() * y);
	}

	Pointer<TPixel> invertedScanline(int invertedY) { return scanline(size().height() - invertedY - 1); }
	Pointer<const TPixel> invertedConstScanline(int invertedY) const { return constScanline(size().height() - invertedY - 1); }

	Pointer<TPixel> pixelPointer(int x, int y)
	{
		Q_ASSERT(0 <= x && x < size().width());
		return scanline(y) + x;
	}
	Pointer<TPixel> pixelPointer(const QPoint &p) { return pixelPointer(p.x(), p.y()); }

	Pointer<const TPixel> constPixelPointer(int x, int y) const
	{
		Q_ASSERT(0 <= x && x < size().width());
		return constScanline(y) + x;
	}
	Pointer<const TPixel> constPixelPointer(const QPoint &p) const { return constPixelPointer(p.x(), p.y()); }

	TPixel pixel(int x, int y) const { return *constPixelPointer(x, y); }
	TPixel pixel(const QPoint &p) const { return pixel(p.x(), p.y()); }

	void setPixel(int x, int y, const TPixel &color)
	{
		auto p = pixelPointer(x, y);
		Q_ASSERT(p);
		*p = color;
	}

	void setPixel(const QPoint &p, const TPixel &color) { setPixel(p.x(), p.y(), color); }

	void fill(const TPixel &c)
	{
		QSize s = size();
		for (int y = 0; y < s.height(); ++y)
		{
			scanline(y).fill(c, s.width());
		}
	}

	template <ImagePasteInversionMode InversionMode = ImagePasteNotInverted, class SrcImage>
	void paste(const SrcImage &image, const QPoint &point = QPoint())
	{
		QRect r = QRect(QPoint(), size()) & QRect(point, image.size());

		for (int y = r.top(); y <= r.bottom(); ++y)
		{
			TPixel *dp;

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

private:

	const TDerived *derived() const { return static_cast<const TDerived *>(this); }
	TDerived *derived() { return static_cast<TDerived *>(this); }

	Pointer<TPixel> bits() { return derived()->bits(); }
	const Pointer<const TPixel> constBits() const { return derived()->constBits(); }
	QSize size() const { return derived()->size(); }
	int bytesPerLine() const { return derived()->bytesPerLine(); }
};

template <typename TPixel>
class MALACHITESHARED_EXPORT Bitmap : public ImageSizeAccessible<Bitmap<TPixel>>, public BitsAccessible<Bitmap<TPixel>, TPixel>
{
public:
	
	using PixelType = TPixel;
	
	Bitmap() :
		_bits(Pointer<PixelType>())
	{}
	
	Bitmap(Pointer<PixelType> bits, const QSize &size, int bytesPerLine) :
		_bits(bits),
		_size(size),
		_bytesPerLine(bytesPerLine)
	{}
	
	void setBits(Pointer<PixelType> *bits) { _bits = bits; }
	void setBits(void *data, int byteCount) { _bits = Pointer<PixelType>(reinterpret_cast<PixelType *>(data), byteCount); }
	
	Pointer<PixelType> bits() { return _bits; }
	const Pointer<const PixelType> constBits() const { return _bits; }
	QSize size() const { return _size; }
	int bytesPerLine() const { return _bytesPerLine; }
	
private:
	
	Pointer<PixelType> _bits;
	QSize _size;
	int _bytesPerLine;
};

}

