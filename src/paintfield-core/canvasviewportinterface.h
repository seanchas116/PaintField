#pragma once

#include <Qt>
#include "global.h"

class QPoint;
class QSize;

namespace Malachite
{
class Affine2D;
class Image;
}

namespace PaintField
{

class CanvasViewportInterface
{
public:
	
	virtual void setScrollBarValue(Qt::Orientation orientation, int value) = 0;
	virtual void setScrollBarRange(Qt::Orientation orientation, int max, int min) = 0;
	virtual void setScrollBarPageStep(Qt::Orientation orientation, int value) = 0;
	
	virtual void setDocumentSize(const QSize &size) = 0;
	virtual void setTransform(const Malachite::Affine2D &transform, bool hasTranslation, bool hasScaling, bool hasRotation) = 0;
	virtual void updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset) = 0;
	virtual void updateAccurately() = 0;
	virtual void beforeUpdateTile() = 0;
	virtual void afterUpdateTile() = 0;
	virtual void update() = 0;
	virtual bool isReady() = 0;
	
	/*
signals:
	void ready();
	void scrollBarXChanged(int);
	void scrollBarYChanged(int);
	
	*/
};

}

Q_DECLARE_INTERFACE(PaintField::CanvasViewportInterface, "PaintField.CanvasViewportInterface")

