#pragma once

#include "canvasviewportsurface.h"

namespace PaintField {

class CanvasViewportMipmap
{
public:
	CanvasViewportMipmap();
	
	void setSceneSize(const QSize &size);
	
	void setCurrentLevel(int level);
	int currentLevel() const { return _currentLevel; }
	
	void replace(const Malachite::ImageU8 &image, const QPoint &key, const QRect &rect);
	
	CanvasViewportSurface surface();
	
private:
	
	void replace(const Malachite::ImageU8 &image, const QPoint &key, const QRect &rect, int maxLevel);
	void update(int maxLevel);
	
	int indexFromKey(const QPoint &key) { return key.y() * _tileCountX + key.x(); }
	void extendSurfaceVector(int max);
	
	int _currentLevel = 0;
	
	int _tileCountX = 0, _tileCountY = 0;
	
	QVector<uint8_t> _maxUpToDateLevels;
	QVector<CanvasViewportSurface> _surfaces;
};

} // namespace PaintField
