#pragma once

#include "global.h"
#include <Malachite/Surface>
#include <QImage>

namespace PaintField {

struct SelectionTileTraits
{
	static constexpr int tileWidth() { return Malachite::Surface::tileWidth(); }
	static uint defaultPixel() { return 0; }
};

class SelectionImage :
	public Malachite::ImageSizeAccessible<SelectionImage>,
	boost::equality_comparable<SelectionImage>
{
public:

	using PixelType = uint;

	SelectionImage(const QSize &size);

	void fill(uint x);
	void paste(const SelectionImage &other, const QPoint &pos);

	QSize size() const { return mImage.size(); }

	QImage &qimage() { return mImage; }
	const QImage &qimage() const { return mImage; }

	bool operator==(const SelectionImage &other) { return mImage == other.mImage; }

private:

	QImage mImage;
};

using SelectionSurface = Malachite::GenericSurface<SelectionImage, SelectionTileTraits>;

} // namespace PaintField
