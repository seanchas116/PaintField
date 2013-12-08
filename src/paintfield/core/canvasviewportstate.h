#pragma once

#include "selectionsurface.h"
#include "canvasviewportmipmap.h"
#include "canvastransforms.h"
#include <QRect>
#include <boost/variant.hpp>

namespace PaintField
{

class Canvas;
class Tool;

class CanvasViewportState
{
public:

	void render(QPainter *painter, const QRect &windowRepaintRect);

	void setCanvas(Canvas *canvas) { this->mCanvas = canvas; }
	void setTool(Tool *tool) { this->mTool = tool; }

	void setTransforms(const SP<const CanvasTransforms> &transforms);
	void setRetinaMode(bool mode) { this->mRetinaMode = mode; }
	void setDocumentSize(const QSize &size);

	QRect updateTiles(const boost::variant<QPointSet, QHash<QPoint, QRect>> &keysOrRectForKeys);
	QRect updateSelectionTiles(const SelectionSurface &surface, const QPointSet &keys);

	CanvasViewportSurface mergedSurface() const { return this->mMipmap.baseSurface(); }

private:

	Canvas *mCanvas = 0;
	Tool *mTool = 0;

	QSize mDocumentSize;
	
	CanvasViewportMipmap mMipmap;
	SelectionMipmap mSelectionMipmap;
	
	SP<const CanvasTransforms> mTransforms;
	
	bool mTranslationOnly = false;
	QPoint mTranslationToScene;
	
	bool mRetinaMode = false;
	
	bool mCacheAvailable = false;
	QRect mCacheRect;
	Malachite::ImageU8 mCacheImage;
};

}
