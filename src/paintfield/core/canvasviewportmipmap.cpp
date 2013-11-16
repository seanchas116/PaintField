#include "canvasviewportmipmap.h"

namespace PaintField {

static QRect alignedHalfRect(const QRect &rect)
{
	int xbegin = rect.left() / 2;
	int xend = (rect.left() + rect.width() + 1) / 2;
	int ybegin = rect.top() / 2;
	int yend = (rect.top() + rect.height() + 1) / 2;
	return QRect(xbegin, ybegin, xend - xbegin, yend - ybegin);
}

// assuming dstRect occupies only one tile in src
static void updateMipmap(CanvasViewportSurface &dst, const QRect &dstRect, const CanvasViewportSurface &src)
{
	constexpr auto tileWidth = CanvasViewportSurface::tileWidth();
	constexpr auto tileWidthHalf = tileWidth / 2;
	
	auto dstKey = QPoint(dstRect.left() / tileWidth, dstRect.top() / tileWidth);
	auto srcKey = QPoint(dstRect.left() / tileWidthHalf, dstRect.top() / tileWidthHalf);
	
	auto &dstTile = dst.tileRef(dstKey);
	auto srcTile = src.tile(srcKey);
	
	auto dstTopLeft = QPoint(dstRect.left() % tileWidth, dstRect.top() % tileWidth);
	auto srcTopLeft = QPoint(dstRect.left() % tileWidthHalf, dstRect.top() % tileWidthHalf) * 2;
	
	for (int ycount = 0; ycount < dstRect.height(); ++ycount)
	{
		auto dstPos = dstTopLeft + QPoint(0, ycount);
		auto srcPos0 = srcTopLeft + QPoint(0, 2 * ycount);
		auto srcPos1 = srcPos0 + QPoint(0, 1);
		auto dstScanline = dstTile.pixelPointer(dstPos);
		auto srcScanline0 = srcTile.constPixelPointer(srcPos0);
		auto srcScanline1 = srcTile.constPixelPointer(srcPos1);
		
		for (int xcount = 0; xcount < dstRect.width(); ++xcount)
		{
			auto s0 = srcScanline0[0], s1 = srcScanline0[1], s2 = srcScanline1[0], s3 = srcScanline1[1];
			
			int r = int(s0.r()) + int(s1.r()) + int(s2.r()) + int(s3.r());
			int g = int(s0.g()) + int(s1.g()) + int(s2.g()) + int(s3.g());
			int b = int(s0.b()) + int(s1.b()) + int(s2.b()) + int(s3.b());
			r /= 4;
			g /= 4;
			b /= 4;
			
			CanvasViewportSurface::PixelType result;
			result.setA(255);
			result.setR(r);
			result.setG(g);
			result.setB(b);
			
			*dstScanline = result;
			dstScanline++;
			srcScanline0 += 2;
			srcScanline1 += 2;
		}
	}
}

static QRect rectForLevel(const QRect &originalRect, int level)
{
	if (level == 0)
		return originalRect;
	if (level == 1)
		return alignedHalfRect(originalRect);
	else
		return rectForLevel(alignedHalfRect(originalRect), level - 1);
}

CanvasViewportMipmap::CanvasViewportMipmap() :
    _surfaces(8)
{
}

void CanvasViewportMipmap::setCurrentLevel(int level)
{
	if (_currentLevel != level)
	{
		extendSurfaceVector(level);
		_currentLevel = level;
		update(level);
	}
}

void CanvasViewportMipmap::setSceneSize(const QSize &size)
{
	constexpr auto tileWidth = CanvasViewportSurface::tileWidth();
	_tileCountX = (size.width() - 1) / tileWidth + 1;
	_tileCountY = (size.height() - 1) / tileWidth + 1;
	_maxUpToDateLevels.resize(_tileCountX * _tileCountY);
	_maxUpToDateLevels.fill(0);
}

void CanvasViewportMipmap::replace(const Malachite::ImageU8 &image, const QPoint &key, const QRect &rect)
{
	replace(image, key, rect, _currentLevel);
}

void CanvasViewportMipmap::replace(const Malachite::ImageU8 &image, const QPoint &key, const QRect &rect, int maxLevel)
{
	Q_ASSERT(_surfaces.size() > maxLevel);
	
	// paste image to base surface
	_surfaces[0].tileRef(key).paste(image, rect.topLeft());
	
	auto globalRect = rect.translated(key * CanvasViewportSurface::tileWidth());
	
	// update mipmaps until level reaches given maxLevel
	for (int level = 1; level <= maxLevel; ++level)
		updateMipmap(_surfaces[level], rectForLevel(globalRect, level), _surfaces[level - 1]);
	
	_maxUpToDateLevels[indexFromKey(key)] = maxLevel;
}

void CanvasViewportMipmap::update(int maxLevel)
{
	Q_ASSERT(_surfaces.size() > maxLevel);
	
	constexpr auto tileWidth = CanvasViewportSurface::tileWidth();
	
	for (auto tileY = 0; tileY < _tileCountY; ++tileY)
	{
		for (auto tileX = 0; tileX < _tileCountX; ++tileX)
		{
			auto key = QPoint(tileX, tileY);
			auto alreadyUpToDateLevel = _maxUpToDateLevels[indexFromKey(key)];
			
			// update mipmap for each level if needed
			for (int level = alreadyUpToDateLevel + 1; level <= maxLevel; ++level)
			{
				auto rect = QRect(key * tileWidth, QSize(tileWidth, tileWidth));
				updateMipmap(_surfaces[level], rectForLevel(rect, level), _surfaces[level - 1]);
			}
			
			// set max up-to-date level if needed
			if (alreadyUpToDateLevel < maxLevel)
				_maxUpToDateLevels[indexFromKey(key)] = maxLevel;
		}
	}
}

CanvasViewportSurface CanvasViewportMipmap::surface() const
{
	Q_ASSERT(_surfaces.size() > _currentLevel);
	
	return _surfaces.at(_currentLevel);
}

CanvasViewportSurface CanvasViewportMipmap::baseSurface() const
{
	return _surfaces.at(0);
}

void CanvasViewportMipmap::extendSurfaceVector(int max)
{
	if (_surfaces.size() <= max)
		_surfaces.resize(max + 1);
}

} // namespace PaintField
