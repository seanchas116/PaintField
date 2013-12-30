#include "selectionsurface.h"
#include <boost/range/irange.hpp>

namespace PaintField {

SelectionImage::SelectionImage(const QSize &size) :
	mImage(size, QImage::Format_Mono)
{
}

void SelectionImage::fill(bool x)
{
	mImage.fill(x);
}

void SelectionImage::paste(const SelectionImage &other, const QPoint &pos)
{
	QPainter painter(&mImage);
	painter.drawImage(pos, other.mImage);
}

bool SelectionImage::isBlank() const
{
	int h = height();
	int w = width();
	for (int y : boost::irange(0, h)) {
		auto i = constScanline(y);
		for (int x : boost::irange(0, w)) {
			if (*i++)
				return false;
		}
	}
	return true;
}

QImage SelectionImage::toQImageARGBPremult(QRgb rgbOne, QRgb rgbZero) const
{
	auto size = this->size();
	QImage image(size, QImage::Format_ARGB32_Premultiplied);
	std::array<uint32_t, 2> rgbs = {{ rgbZero, rgbOne }};

	for (int y = 0; y < size.height(); ++y) {
		auto dst = reinterpret_cast<uint32_t *>(image.scanLine(y));
		auto dstEnd = dst + size.width();
		auto src = this->constScanline(y);
		while (dst != dstEnd) {
			*dst++ = rgbs[*src++];
		}
	}

	return image;
}


namespace SelectionDrawUtil {

QPointSet drawPath(SelectionSurface &surface, const QPainterPath &path, Mode mode)
{
	auto keys = SelectionSurface::rectToKeys(path.boundingRect().toAlignedRect());

	for (const QPoint &key : keys) {

		auto rect = SelectionSurface::keyToRect(key);
		if (path.contains(rect)) {
			surface[key] =
				(mode == Mode::Draw) ? filledTile() : SelectionSurface::defaultTile();
			continue;
		}

		QPainterPath rectPath;
		rectPath.addRect(rect);
		auto dividedPath = path & rectPath;

		if (dividedPath.isEmpty())
			continue;

		auto &tile = surface.tileRef(key);
		QPainter painter(&tile.qimage());
		painter.setPen(Qt::NoPen);
		painter.translate(-key * SelectionSurface::tileWidth());
		painter.setCompositionMode(
			(mode == Mode::Draw)
				? QPainter::CompositionMode_SourceOver
				: QPainter::CompositionMode_DestinationOut);
		painter.drawPath(path);
	}
	return keys;
}

static SelectionImage makeFilledTile()
{
	SelectionImage tile(SelectionSurface::tileSize());
	tile.fill(true);
	return tile;
}

static SelectionImage filledTile_ = makeFilledTile();

SelectionImage filledTile()
{
	return filledTile_;
}

}

} // namespace PaintField
