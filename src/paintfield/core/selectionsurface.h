#pragma once

#include "global.h"
#include <Malachite/Surface>
#include <Malachite/SurfaceMipmap>
#include <QImage>

namespace PaintField {

namespace detail {

struct SelectionTileTraits
{
	static constexpr int tileWidth() { return Malachite::Surface::tileWidth(); }
	static bool defaultPixel() { return false; }
};

struct SelectionIteratorConstTag {};
struct SelectionIteratorMutableTag {};

template <class TConstnessTag>
class SelectionIteratorReference;

template <>
class SelectionIteratorReference<SelectionIteratorMutableTag>
{
public:
	SelectionIteratorReference(uint8_t &byte, uint8_t bitMask) :
		mByte(byte), mBitMask(bitMask) {}

	operator bool() const
	{
		return mByte & mBitMask;
	}

	SelectionIteratorReference &operator=(bool value)
	{
		if (value)
			mByte |= mBitMask;
		else
			mByte &= ~mBitMask;
		return *this;
	}

private:
	uint8_t &mByte;
	uint8_t mBitMask;
};

template <>
class SelectionIteratorReference<SelectionIteratorConstTag>
{
public:
	SelectionIteratorReference(const uint8_t &byte, uint8_t bitMask) :
		mByte(byte), mBitMask(bitMask) {}

	operator bool() const
	{
		return mByte & mBitMask;
	}

private:
	const uint8_t &mByte;
	uint8_t mBitMask;
};

}

template <class TConstnessTag>
class SelectionIterator :
	public boost::iterator_facade<
		SelectionIterator<TConstnessTag>,
		bool,
		boost::random_access_traversal_tag,
		detail::SelectionIteratorReference<TConstnessTag>>
{
	template <class TMutable, class TConst>
	using ConstnessConditional = typename std::conditional<
		std::is_same<TConstnessTag, detail::SelectionIteratorMutableTag>::value,
		TMutable, TConst>::type;

public:

	SelectionIterator(ConstnessConditional<QImage &, const QImage &> image, int y)
	{
		Q_ASSERT(image.format() == QImage::Format_Mono);
		mByte = image.scanLine(y);
	}

private:

	friend class boost::iterator_core_access;

	detail::SelectionIteratorReference<TConstnessTag> dereference() const
	{
		return detail::SelectionIteratorReference<TConstnessTag>(*mByte, mBitMask);
	}

	void increment()
	{
		if (mBitMask == 0x01) {
			mBitMask = 0x80;
			++mByte;
		} else {
			mBitMask >>= 1;
		}
	}

	void decrement()
	{
		if (mBitMask == 0x80) {
			mBitMask = 0x01;
			--mByte;
		} else {
			mBitMask <<= 1;
		}
	}

	bool equal(const SelectionIterator &other) const
	{
		return mByte == other.mByte && mBitMask == other.mBitMask;
	}

	void advance(std::ptrdiff_t n)
	{
		auto divided = std::div(n, std::ptrdiff_t(8));
		mByte += divided.quot;
		auto r = divided.rem;
		if (r >= 0) {
			while (r--)
				increment();
		} else {
			while (r++)
				decrement();
		}
	}

	ConstnessConditional<uint8_t *, const uint8_t *> mByte;
	uint8_t mBitMask = 0x80;
};

class SelectionImage :
	public Malachite::ImageSizeAccessible<SelectionImage>,
	public Malachite::ImagePixelsAccessible<
		SelectionImage,
		SelectionIterator<detail::SelectionIteratorMutableTag>,
		SelectionIterator<detail::SelectionIteratorConstTag>>,
	boost::equality_comparable<SelectionImage>
{
public:

	using PixelType = bool;
	using value_type = PixelType;
	using iterator = SelectionIterator<detail::SelectionIteratorMutableTag>;
	using const_iterator = SelectionIterator<detail::SelectionIteratorConstTag>;

	SelectionImage() = default;
	SelectionImage(const QSize &size);

	void fill(bool x);
	void paste(const SelectionImage &other, const QPoint &pos);

	QSize size() const { return mImage.size(); }

	bool isBlank() const;

	iterator scanline(int y)
	{
		return iterator(mImage, y);
	}

	const_iterator constScanline(int y) const
	{
		return const_iterator(mImage, y);
	}

	QImage &qimage() { return mImage; }
	const QImage &qimage() const { return mImage; }

	QImage toQImageArgbPremult(QRgb rgbOne, QRgb rgbZero = 0) const;

	// result image is narrowed by 1px
	QImage toDottedQImage(int width, const QPoint &offset) const;

	bool operator==(const SelectionImage &other) const { return mImage == other.mImage; }

private:

	QImage mImage;
};

using SelectionSurface = Malachite::GenericSurface<SelectionImage, detail::SelectionTileTraits>;

struct SelectionMipmapPixelTraits
{
	static bool average(const std::array<bool, 4> &pixels)
	{
		int sum = 0;
		for (auto pixel : pixels) {
			sum += static_cast<int>(pixel);
		}
		return sum >= 2;
	}
};

using SelectionMipmap = Malachite::SurfaceMipmap<SelectionSurface, SelectionMipmapPixelTraits>;


namespace SelectionDrawUtil {

enum class Mode {
	Draw, Erase
};

QPointSet drawPath(SelectionSurface &surface, const QPainterPath &path, Mode mode = Mode::Draw);
SelectionImage filledTile();

}

} // namespace PaintField
