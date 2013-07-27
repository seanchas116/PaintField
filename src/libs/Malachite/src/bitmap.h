#pragma once

//ExportName: Bitmap

#include <QSize>
#include <QRect>
#include "misc.h"
#include "memory.h"

namespace Malachite
{

template <typename T_Pixel>
class MALACHITESHARED_EXPORT Bitmap
{
public:
	
	typedef T_Pixel PixelType;
	
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
	int width() const { return _size.width(); }
	int height() const { return _size.height(); }
	int byteCount() const { return _bytesPerLine * _size.height(); }
	int bytesPerLine() const { return _bytesPerLine; }
	int area() const { return _size.height() * _size.width(); }
	QRect rect() const { return QRect(QPoint(), _size); }
	
	Pointer<PixelType> scanline(int y)
	{
		Q_ASSERT(0 <= y && y < _size.height());
		return _bits.byteOffset(_bytesPerLine * y);
	}
	Pointer<const PixelType> constScanline(int y) const
	{
		Q_ASSERT(0 <= y && y < _size.height());
		return _bits.byteOffset(_bytesPerLine * y);
	}
	
	Pointer<PixelType> invertedScanline(int invertedY) { return scanline(height() - invertedY - 1); }
	Pointer<const PixelType> invertedConstScanline(int invertedY) const { return constScanline(height() - invertedY - 1); }
	
	Pointer<PixelType> pixelPointer(int x, int y)
	{
		Q_ASSERT(0 <= x && x < _size.width());
		return scanline(y) + x;
	}
	Pointer<PixelType> pixelPointer(const QPoint &p) { return pixelPointer(p.x(), p.y()); }
	
	Pointer<const PixelType> constPixelPointer(int x, int y) const
	{
		Q_ASSERT(0 <= x && x < _size.width());
		return constScanline(y) + x;
	}
	Pointer<const PixelType> constPixelPointer(const QPoint &p) const { return constPixelPointer((p.x(), p.y())); }
	
	PixelType pixel(int x, int y) const { return *constPixelPointer(x, y); }
	PixelType pixel(const QPoint &p) const { return pixel(p.x(), p.y()); }
	
private:
	
	Pointer<PixelType> _bits;
	QSize _size;
	int _bytesPerLine;
};

}

