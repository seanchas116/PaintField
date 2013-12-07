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

/*
template <typename TFunction>
void drawDivided(const QRect &viewRect, const TFunction &drawFunc)
{
	constexpr auto unit = 128;
	
	if (viewRect.width() * viewRect.height() <= unit * unit)
	{
		drawFunc(viewRect);
	}
	else
	{
		int xCount = viewRect.width() / unit;
		if (viewRect.width() % unit)
			xCount++;
		
		int yCount = viewRect.height() / unit;
		if (viewRect.height() % unit)
			yCount++;
		
		for (int x = 0; x < xCount; ++x)
		{
			for (int y = 0; y < yCount; ++y)
			{
				auto viewRectDivided = viewRect & QRect(viewRect.topLeft() + QPoint(x, y) * unit, QSize(unit, unit));
				drawFunc(viewRectDivided);
			}
		}
	}
}

template <typename TDrawImageFunction, typename TDrawBackgroundFunction>
void drawViewport(const QRect &windowRepaintRect, CanvasViewportState *state, const TDrawImageFunction &drawImage, const TDrawBackgroundFunction &drawBackground)
{
	bool retinaMode = state->mRetinaMode;
	
	auto fromWindowRect = [retinaMode](const QRect &rect)->QRect
	{
		if (retinaMode)
			return QRect(rect.left() * 2, rect.top() * 2, rect.width() * 2, rect.height() * 2);
		else
			return rect;
	};
	
	auto toWindowRect = [retinaMode](const QRect &rect)->QRect
	{
		if (retinaMode)
			return QRect(rect.left() / 2, rect.top() / 2, rect.width() / 2, rect.height() / 2);
		else
			return rect;
	};
	
	auto repaintRect = fromWindowRect(windowRepaintRect);
	auto surface = state->mMipmap.surface();
	auto selectionSurface = state->mSelectionMipmap.surface();

	auto cropSurface = [&](const QRect &rect)
	{
		//PAINTFIELD_DEBUG << state->cacheRect << rect;
		if (state->mCacheAvailable && state->mCacheRect == rect) {
			//PAINTFIELD_DEBUG << "cached";
			return state->mCacheImage;
		} else {
			return surface.crop(rect);
		}
	};
	
	if (state->mTranslationOnly) // easy, view is only translated
	{
		//PAINTFIELD_DEBUG << "translation only";
		
		auto drawInViewRect = [&](const QRect &viewRect)
		{
			auto sceneRect = viewRect.translated(state->mTranslationToScene);
			auto windowRect = toWindowRect(viewRect);
			
			if ((sceneRect & QRect(QPoint(), state->mDocumentSize)).isEmpty()) {
				drawBackground(windowRect);
			} else {
				drawImage(windowRect, Malachite::wrapInQImage(cropSurface(sceneRect)));
				drawImage(windowRect, selectionSurface.crop(sceneRect).toQImageARGBPremult(qRgb(255, 0, 0)));
			}
		};

		drawDivided(repaintRect, drawInViewRect);
	}
	else
	{
		//PAINTFIELD_DEBUG << "transformed";
		
		auto drawInViewRect = [&](const QRect &viewRect)
		{
			auto sceneRect = state->mTransforms->viewToMipmap.mapRect(QRectF(viewRect)).toAlignedRect();
			auto croppedImage = cropSurface(sceneRect);
			
			QImage image(viewRect.size(), QImage::Format_ARGB32_Premultiplied);
			{
				QPainter imagePainter(&image);
				imagePainter.setCompositionMode(QPainter::CompositionMode_Source);
				
				if (state->mTransforms->scale < 2.0)
					imagePainter.setRenderHint(QPainter::SmoothPixmapTransform);
				
				imagePainter.setTransform( state->mTransforms->mipmapToView * QTransform::fromTranslate(-viewRect.left(), -viewRect.top()) );
				imagePainter.drawImage(sceneRect.topLeft(), wrapInQImage(croppedImage));
			}
			
			if ((sceneRect & QRect(QPoint(), state->mDocumentSize)).isEmpty())
				drawBackground(toWindowRect(viewRect));
			else
				drawImage(toWindowRect(viewRect), image);
		};
		
		drawDivided(repaintRect, drawInViewRect);
	}
}*/

}
