#pragma once

#include "canvasviewportsurface.h"

namespace PaintField {

class CanvasViewportMipmap
{
public:
	CanvasViewportMipmap();
	
	void setSceneSize(const QSize &size);
	
	void setCurrentLevel(int level);
	int currentLevel() const { return mCurrentLevel; }
	
	void replace(const Malachite::ImageU8 &image, const QPoint &key, const QRect &rect);
	
	CanvasViewportSurface surface() const;
	CanvasViewportSurface baseSurface() const;
	
private:
	
	void replace(const Malachite::ImageU8 &image, const QPoint &key, const QRect &rect, int maxLevel);
	void update(int maxLevel);
	
	int indexFromKey(const QPoint &key) { return key.y() * mTileCountX + key.x(); }
	void extendSurfaceVector(int max);
	
	int mCurrentLevel = 0;
	
	int mTileCountX = 0, mTileCountY = 0;
	
	QVector<uint8_t> mMaxUpToDateLevels;
	QVector<CanvasViewportSurface> mSurfaces;
};

} // namespace PaintField
