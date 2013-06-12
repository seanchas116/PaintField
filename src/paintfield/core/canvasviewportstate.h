#pragma once

#include <QRect>
#include <QImage>
#include <QPainter>
#include "canvas.h"

#include "canvasviewportmipmap.h"

namespace PaintField
{

struct CanvasViewportState
{
	QSize documentSize;
	
	//CanvasViewportSurface surface;
	CanvasViewportMipmap mipmap;
	
	std::shared_ptr<const CanvasTransforms> transforms;
	
	bool translationOnly = false;
	QPoint translationToScene;
	
	bool retinaMode = false;
	
	bool cacheAvailable = false;
	QRect cacheRect;
	Malachite::ImageU8 cacheImage;
};

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
	bool retinaMode = state->retinaMode;
	
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
	auto surface = state->mipmap.surface();
	
	auto cropSurface = [&](const QRect &rect)
	{
		if (state->cacheAvailable && state->cacheRect == rect)
			return state->cacheImage;
		else
			return surface.crop(rect);
	};
	
	if (state->translationOnly) // easy, view is only translated
	{
		PAINTFIELD_DEBUG << "translation only";
		
		auto drawInViewRect = [&](const QRect &viewRect)
		{
			auto sceneRect = viewRect.translated(state->translationToScene);
			
			if ((sceneRect & QRect(QPoint(), state->documentSize)).isEmpty())
				drawBackground(toWindowRect(viewRect));
			else
				drawImage(toWindowRect(viewRect), cropSurface(sceneRect).wrapInQImage());
		};
		
		drawDivided(repaintRect, drawInViewRect);
	}
	else
	{
		PAINTFIELD_DEBUG << "transformed";
		
		auto drawInViewRect = [&](const QRect &viewRect)
		{
			auto sceneRect = state->transforms->viewToMipmap.mapRect(QRectF(viewRect)).toAlignedRect();
			auto croppedImage = cropSurface(sceneRect);
			
			QImage image(viewRect.size(), QImage::Format_ARGB32_Premultiplied);
			{
				QPainter imagePainter(&image);
				imagePainter.setCompositionMode(QPainter::CompositionMode_Source);
				
				if (state->transforms->scale < 2.0)
					imagePainter.setRenderHint(QPainter::SmoothPixmapTransform);
				
				imagePainter.setTransform( state->transforms->mipmapToView * QTransform::fromTranslate(-viewRect.left(), -viewRect.top()) );
				imagePainter.drawImage(sceneRect.topLeft(), croppedImage.wrapInQImage());
			}
			
			if ((sceneRect & QRect(QPoint(), state->documentSize)).isEmpty())
				drawBackground(toWindowRect(viewRect));
			else
				drawImage(toWindowRect(viewRect), image);
		};
		
		drawDivided(repaintRect, drawInViewRect);
	}
}

}
