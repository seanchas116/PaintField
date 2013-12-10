#pragma once

//ExportName: SurfaceMipmap

#include "misc.h"
#include "pixel.h"
#include <algorithm>
#include <boost/range/irange.hpp>

namespace Malachite {

template <class TPixel>
struct MipmapPixelTraits;

template <>
struct MipmapPixelTraits<Pixel>
{
	static Pixel average(const std::array<Pixel, 4> &pixels)
	{
		Pixel sum(0.f);
		for (const auto &pixel : pixels) {
			sum.rv() += pixel.v();
		}
		sum.rv() *= 0.25f;
		return sum;
	}
};

template <>
struct MipmapPixelTraits<BgraPremultU8>
{
	static Pixel average(const std::array<BgraPremultU8, 4> &pixels)
	{
		int a = 0;
		int r = 0;
		int g = 0;
		int b = 0;
		for (const auto &pixel : pixels) {
			a += int(pixel.a());
			r += int(pixel.r());
			g += int(pixel.g());
			b += int(pixel.b());
		}
		a /= 4;
		r /= 4;
		g /= 4;
		b /= 4;
		BgraPremultU8 result;
		result.setA(a);
		result.setR(r);
		result.setG(g);
		result.setB(b);
		return result;
	}
};

template <
	class TSurface,
	class TMipmapPixelTraits = MipmapPixelTraits<typename TSurface::PixelType>,
	int TDepth = 8
	>
class SurfaceMipmap
{
public:

	static_assert(TDepth > 0, "mipmap depth must be positive");

	constexpr static int depth() { return TDepth; }

	SurfaceMipmap() :
		mSurfaces(depth())
	{
	}

	void setSceneSize(const QSize &size)
	{
		constexpr auto tileWidth = TSurface::tileWidth();
		mTileCountX = (size.width() - 1) / tileWidth + 1;
		mTileCountY = (size.height() - 1) / tileWidth + 1;
		mMaxUpToDateLevels.resize(mTileCountX * mTileCountY);
		mMaxUpToDateLevels.fill(0);
		update(mCurrentLevel);
	}

	void setCurrentLevel(int level)
	{
		if (mCurrentLevel != level)
		{
			extendSurfaceVector(level);
			mCurrentLevel = level;
			update(level);
		}
	}

	int currentLevel() const { return mCurrentLevel; }

	void replace(const typename TSurface::ImageType &image, const QPoint &key, const QPoint &relativePos = QPoint())
	{
		if (!checkKey(key)) return;

		auto size = image.size();
		if (size == TSurface::tileSize() && relativePos == QPoint()) {
			mSurfaces[0].setTile(key, image);
		} else {
			mSurfaces[0].tileRef(key).paste(image, relativePos);
		}

		auto globalRect = QRect(relativePos, size).translated(key * TSurface::tileWidth());

		update(globalRect, 0, mCurrentLevel);

		mMaxUpToDateLevels[indexFromKey(key)] = mCurrentLevel;
	}

	void replace(const TSurface &surface, const QPointSet &keys)
	{
		mSurfaces[0].replace(surface, keys);
		for (const auto &key : keys) {
			if (!checkKey(key)) continue;
			auto rect = QRect(key * TSurface::tileWidth(), TSurface::tileSize());
			update(rect, 0, mCurrentLevel);
			mMaxUpToDateLevels[indexFromKey(key)] = mCurrentLevel;
		}
	}

	TSurface surface() const
	{
		return mSurfaces.at(mCurrentLevel);
	}

	TSurface baseSurface() const
	{
		return mSurfaces.at(0);
	}

private:

	bool checkKey(const QPoint &key) const
	{
		return 0 <= key.x() && key.x() < mTileCountX && 0 <= key.y() && key.y() < mTileCountY;
	}

	void update(const QRect &rect, int alreadyUpToDateLevel, int maxLevel)
	{
		auto rectForLevel = rect;

		for (int level = 0; level <= maxLevel; ++level) {
			if (level > alreadyUpToDateLevel)
				updateMipmap(mSurfaces[level], rectForLevel, mSurfaces[level - 1]);
			rectForLevel = alignedHalfRect(rectForLevel);
		}
	}

	void update(int maxLevel)
	{
		Q_ASSERT(mSurfaces.size() > maxLevel);

		constexpr auto tileWidth = TSurface::tileWidth();

		for (auto tileY = 0; tileY < mTileCountY; ++tileY)
		{
			for (auto tileX = 0; tileX < mTileCountX; ++tileX)
			{
				auto key = QPoint(tileX, tileY);
				auto alreadyUpToDateLevel = mMaxUpToDateLevels[indexFromKey(key)];

				auto rect = QRect(key * tileWidth, QSize(tileWidth, tileWidth));
				update(rect, alreadyUpToDateLevel, maxLevel);

				// set max up-to-date level if needed
				if (alreadyUpToDateLevel < maxLevel)
					mMaxUpToDateLevels[indexFromKey(key)] = maxLevel;
			}
		}
	}

	int indexFromKey(const QPoint &key) { return key.y() * mTileCountX + key.x(); }

	void extendSurfaceVector(int max)
	{
		if (mSurfaces.size() <= max)
			mSurfaces.resize(max + 1);
	}

	static void updateMipmap(TSurface &dst, const QRect &dstRect, const TSurface &src)
	{
		constexpr auto tileWidth = TSurface::tileWidth();
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
				std::array<typename TSurface::PixelType, 4> srcs;
				srcs[0] = *srcScanline0++;
				srcs[1] = *srcScanline0++;
				srcs[2] = *srcScanline1++;
				srcs[3] = *srcScanline1++;
				*dstScanline = TMipmapPixelTraits::average(srcs);
				dstScanline++;
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

	int mCurrentLevel = 0;
	int mTileCountX = 0, mTileCountY = 0;

	QVector<uint8_t> mMaxUpToDateLevels;
	QVector<TSurface> mSurfaces;
};


}

