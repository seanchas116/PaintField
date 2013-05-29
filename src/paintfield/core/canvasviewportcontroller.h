#pragma once

#include <QObject>
#include <QRect>
#include <Malachite/Surface>
#include <QImage>
#include <QPainter>
#include "global.h"

#if defined(Q_OS_MAC) && !defined(PF_FORCE_RASTER_ENGINE)
#define PF_CANVAS_VIEWPORT_COCOA
#endif

class QPoint;
class QSize;

namespace Malachite
{
class Affine2D;
class Image;
}

namespace PaintField {

class CanvasViewportController : public QObject
{
	Q_OBJECT
public:
	explicit CanvasViewportController(QObject *parent = 0);
	~CanvasViewportController();
	
	void beginUpdateTile(int tileCount);
	void updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset);
	void endUpdateTile();
	
	void placeViewport(QWidget *window);
	void moveViewport(const QRect &rect, bool visible);
	
signals:
	
	void viewSizeChanged(const QSize &size);
	
public slots:
	
	void setTransform(const Malachite::Affine2D &toScene, const Malachite::Affine2D &fromScene);
	void setRetinaMode(bool mode);
	void setDocumentSize(const QSize &size);
	void update();
	
private:
	
	struct Data;
	Data *d;
};

struct CanvasViewportTileTraits
{
	static constexpr int tileWidth() { return Malachite::Surface::tileWidth(); }
	static Malachite::ImageU8::PixelType defaultPixel() { return Malachite::ImageU8::PixelType(128, 128, 128, 255); }
};

typedef Malachite::GenericSurface<Malachite::ImageU8, CanvasViewportTileTraits> CanvasViewportSurface;

struct CanvasViewportState
{
	CanvasViewportSurface surface;
	
	QTransform transformToScene, transformToView;
	
	bool translationOnly = false;
	QPoint translationToScene;
	
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

template <typename TDrawFunction>
void drawViewport(const QRect &repaintRect, CanvasViewportState *state, const TDrawFunction &drawFunc)
{
	auto cropSurface = [&](const QRect &rect)
	{
		if (state->cacheAvailable && state->cacheRect == rect)
			return state->cacheImage;
		else
			return state->surface.crop<Malachite::ImageU8>(rect);
	};
	
	if (state->translationOnly) // easy, view is only translated
	{
		auto drawInViewRect = [&](const QRect &viewRect)
		{
			auto sceneRect = viewRect.translated(state->translationToScene);
			drawFunc(viewRect, cropSurface(sceneRect).wrapInQImage());
		};
		
		drawDivided(repaintRect, drawInViewRect);
	}
	else
	{
		auto drawInViewRect = [&](const QRect &viewRect)
		{
			auto sceneRect = state->transformToScene.mapRect(QRectF(viewRect)).toAlignedRect();
			auto croppedImage = cropSurface(sceneRect);
			
			QImage image(viewRect.size(), QImage::Format_ARGB32_Premultiplied);
			{
				QPainter imagePainter(&image);
				imagePainter.setCompositionMode(QPainter::CompositionMode_Source);
				imagePainter.setRenderHint(QPainter::SmoothPixmapTransform);
				imagePainter.setTransform( state->transformToView * QTransform::fromTranslate(-viewRect.left(), -viewRect.top()) );
				imagePainter.drawImage(sceneRect.topLeft(), croppedImage.wrapInQImage());
			}
			
			drawFunc(viewRect, image);
		};
		
		drawDivided(repaintRect, drawInViewRect);
	}
}

} // namespace PaintField
