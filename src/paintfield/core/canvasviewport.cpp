#include <QPainter>
#include <Malachite/Image>
#include <Malachite/Surface>
#include <Malachite/Affine2D>
#include <Malachite/Interval>
#include <QPaintEvent>

#include "scopedtimer.h"

#include "canvasviewport.h"

using namespace Malachite;

namespace PaintField {

struct CanvasViewportOld::Data
{
	QTransform transformOutputFromScene, transformSceneFromOutput;
	bool transformTranslatingOnly = true, retinaMode = false;
	QPoint translation;
	
	QRect outputRectAlignedForWidget(const QRect &rect) const
	{
		if (retinaMode)
		{
			int startX = rect.left();
			int endX = startX + rect.width();
			int startY = rect.top();
			int endY = startY + rect.height();
			
			if (startX % 2)
				--startX;
			if (startY % 2)
				--startY;
			if (endX % 2)
				++endX;
			if (endY % 2)
				++endY;
			
			return QRect(startX, startY, endX - startX, endY - startY);
		}
		else
		{
			return rect;
		}
	}
	
	QRect sceneRectFromOutputRect(const QRect &rect) const
	{
		return transformSceneFromOutput.mapRect(QRectF(rect)).toAlignedRect();
	}
	
	QRect outputRectFromSceneRect(const QRect &rect) const
	{
		return transformOutputFromScene.mapRect(QRectF(rect)).toAlignedRect();
	}
	
	QRect widgetRectFromOutputRect(const QRect &rect) const
	{
		if (retinaMode)
			return QRectF(rect.x() * 0.5, rect.y() * 0.5, rect.width() * 0.5, rect.height() * 0.5).toAlignedRect();
		else
			return rect;
	}
	
	QRect outputRectFromWidgetRect(const QRect &rect) const
	{
		if (retinaMode)
			return QRect(rect.x() * 2, rect.y() * 2, rect.width() * 2, rect.height() * 2);
		else
			return rect;
	}
	
	QRect outputRect;
	
	SurfaceU8 surface;
	QSize sceneSize;
	int tileRight, tileBottom;
	QPointSet tileKeys;
	
	UpdateMode updateMode = FastUpdate;
	
	QVector<QRect> accurateUpdateOutputRects;
	QRect unionAccurateUpdateOutputRect;
	bool accurateUpdateConsiderBorder = false;
	
	void resetUpdate()
	{
		accurateUpdateOutputRects.clear();
		accurateUpdateConsiderBorder = false;
		updateMode = FastUpdate;
	}
	
	QPixmap fastUpdatePixmap;
	QPointSet fastUpdatePixelUnpaintedTiles;
	
	void paintUnpaintedTilesToPixmap()
	{
		if (fastUpdatePixelUnpaintedTiles.isEmpty())
			return;
		
		QPainter painter(&fastUpdatePixmap);
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		
		for (const auto &key : fastUpdatePixelUnpaintedTiles)
			painter.drawImage( key * Surface::tileWidth(), surface.tileRef(key).wrapInQImage() );
		
		fastUpdatePixelUnpaintedTiles.clear();
	}
};

CanvasViewportOld::CanvasViewportOld(QWidget *parent) :
	QWidget(parent),
	d(new Data)
{
}

CanvasViewportOld::~CanvasViewportOld()
{
	delete d;
}

void CanvasViewportOld::setDocumentSize(const QSize &size)
{
	d->sceneSize = size;
	d->tileKeys = Surface::rectToKeys(QRect(QPoint(), size));
	d->fastUpdatePixmap = QPixmap(size);
	
	d->tileRight = (size.width() - 1) / Surface::tileWidth();
	d->tileBottom = (size.height() - 1) / Surface::tileWidth();
}

void CanvasViewportOld::setTransform(const Affine2D &transform, const QPoint &translation, double scale, double rotation, bool retinaMode)
{
	Q_UNUSED(translation)
	
	d->retinaMode = retinaMode;
	
	if (retinaMode)
		d->transformOutputFromScene = (Affine2D::fromScale(2) * transform).toQTransform();
	else
		d->transformOutputFromScene = transform.toQTransform();
	
	d->transformSceneFromOutput = d->transformOutputFromScene.inverted();
	
	if (retinaMode)
		d->transformTranslatingOnly = (scale == 0.5 && rotation == 0);
	else
		d->transformTranslatingOnly = (scale == 1.0 && rotation == 0);
	
	d->translation = translation;
}

void CanvasViewportOld::beforeUpdateTile(UpdateMode mode, int updateTileCount)
{
	d->updateMode = mode;
	d->accurateUpdateOutputRects.reserve(updateTileCount);
}

void CanvasViewportOld::updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset)
{
	d->fastUpdatePixelUnpaintedTiles << tileKey;
	
	d->surface.tileRef(tileKey).paste(image.toImageU8(), offset);
	
	QPoint pos = tileKey * Surface::tileWidth() + offset;
	auto outputRect = d->outputRectFromSceneRect(QRect(pos, image.size())) & d->outputRect;
	
	if (outputRect.isEmpty())
		return;
	
	outputRect = d->outputRectAlignedForWidget(outputRect);
	
	d->accurateUpdateOutputRects << outputRect;
	d->accurateUpdateConsiderBorder |= (tileKey.x() <= 0 || tileKey.x() >= d->tileRight || tileKey.y() <= 0 || tileKey.y() >= d->tileBottom);
}

void CanvasViewportOld::afterUpdateTile()
{
	if (d->updateMode != PartialAccurateUpdate)
		return;
	
	QRect unionRect;
	
	for (const auto &rect : d->accurateUpdateOutputRects)
		unionRect |= rect;
	
	d->unionAccurateUpdateOutputRect = unionRect;
	
#ifdef PAINTFIELD_COREGRAPHICS_REPAINT
	
	PAINTFIELD_DEBUG << unionRect;
	
	if (unionRect.width() <= Surface::tileWidth() && unionRect.height() <= Surface::tileWidth())
	{
		d->accurateUpdateOutputRects = { unionRect };
		repaintOutputRect(unionRect);
	}
	else
	{
		for (const auto &rect : d->accurateUpdateOutputRects)
			repaintOutputRect(rect);
	}
	
#else
	if (unionRect.width() <= Surface::tileWidth() && unionRect.height() <= Surface::tileWidth())
	{
		d->accurateUpdateOutputRects = { unionRect };
		repaintOutputRect(unionRect);
		d->resetUpdate();
	}
	else
	{
		auto rects = d->accurateUpdateOutputRects;
		
		for (const auto &rect : rects)
		{
			d->accurateUpdateOutputRects = { rect };
			repaintOutputRect(rect);
		}
		d->resetUpdate();
	}
#endif
}

void CanvasViewportOld::updateWholeAccurately()
{
	d->updateMode = WholeAccurateUpdate;
	repaint();
}

void CanvasViewportOld::paintEvent(QPaintEvent *event)
{
#ifdef PAINTFIELD_COREGRAPHICS_REPAINT
	
	{
		if (d->updateMode == PartialAccurateUpdate && !d->widgetRectFromOutputRect(d->unionAccurateUpdateOutputRect).contains(event->rect()))
			d->updateMode = WholeAccurateUpdate;
	}
	
#else
	
	Q_UNUSED(event);
	
#endif
	
	if (d->updateMode == NoUpdate)
		d->updateMode = FastUpdate;
	
	if (d->updateMode == FastUpdate)
	{
		PAINTFIELD_DEBUG << "fast update";
		
		QPainter painter(this);
		
		if (d->retinaMode)
			painter.setTransform(d->transformOutputFromScene * QTransform::fromScale(0.5, 0.5));
		else
			painter.setTransform(d->transformOutputFromScene);
		
		d->paintUnpaintedTilesToPixmap();
		painter.drawPixmap(QPoint(), d->fastUpdatePixmap);
	}
	else
	{
		if (d->updateMode == WholeAccurateUpdate)
		{
			auto sceneRect = QRect(QPoint(), d->sceneSize);
			auto outputRectInScene = d->sceneRectFromOutputRect(d->outputRect);
			
			QVector<QRect> rects;
			
			for (const QPoint &key : Surface::rectToKeys(sceneRect & outputRectInScene))
				rects << d->outputRectAlignedForWidget(d->outputRectFromSceneRect( Surface::keyToRect( key ) ) );
			
			d->accurateUpdateOutputRects = rects;
			d->accurateUpdateConsiderBorder = true;
		}
		
		PAINTFIELD_DEBUG << "accurate update";
		
		paintRects(d->accurateUpdateOutputRects, d->accurateUpdateConsiderBorder);
		
		// In Core Graphics graphics engine, multiple repaint calls in one event loop are put together
#ifdef PAINTFIELD_COREGRAPHICS_REPAINT
		d->resetUpdate();
#endif
	}
}

void CanvasViewportOld::repaintOutputRect(const QRect &rect)
{
	repaint(d->widgetRectFromOutputRect(rect));
}

void CanvasViewportOld::paintRects(const QVector<QRect> &outputRects, bool considerBorder)
{
	QPainter painter(this);
	if (!considerBorder)
		painter.setCompositionMode(QPainter::CompositionMode_Source);
	
	if (d->transformTranslatingOnly)
	{
		if (d->retinaMode)
			painter.setTransform(d->transformOutputFromScene * QTransform::fromScale(0.5, 0.5));
		else
			painter.setTransform(d->transformOutputFromScene);
		
		for (const QRect &rect : outputRects)
		{
			if (rect.isEmpty())
				continue;
			
			QRect requiredSceneRect = d->sceneRectFromOutputRect(rect);
			
			auto image = d->surface.crop<ImageU8>( requiredSceneRect );
			
			if ( considerBorder )
				painter.setClipRect( QRect( QPoint(), d->sceneSize ) );
			
			painter.drawImage( requiredSceneRect.topLeft(), image.wrapInQImage() );
		}
	}
	else
	{
		if (d->retinaMode)
			painter.scale(0.5, 0.5);
		
		for (const QRect &rect : outputRects)
		{
			if (rect.isEmpty())
				continue;
			
			QRect requiredSceneRect = d->sceneRectFromOutputRect(rect);
			requiredSceneRect.adjust(-1, -1, 1, 1);
			
			auto image = d->surface.crop<ImageU8>( requiredSceneRect );
			
			QImage viewImage( rect.size(), QImage::Format_ARGB32_Premultiplied );
			viewImage.fill( 0 );
			auto viewImageOffset = rect.topLeft();
			
			{
				QPainter imagePainter( &viewImage );
				imagePainter.setRenderHint( QPainter::SmoothPixmapTransform, true );
				imagePainter.setTransform( d->transformOutputFromScene * QTransform::fromTranslate( -viewImageOffset.x(), -viewImageOffset.y() ) );
				imagePainter.drawImage( requiredSceneRect.topLeft(), image.wrapInQImage() );
			}
			
			if ( considerBorder )
			{
				QPainterPath path;
				path.addRect( QRect( QPoint(), d->sceneSize ) );
				painter.setClipPath( path * d->transformOutputFromScene );
			}
			
			painter.drawImage( viewImageOffset, viewImage );
		}
	}
}

void CanvasViewportOld::resizeEvent(QResizeEvent *)
{
	QRect widgetRect(QPoint(), size());
	d->outputRect = d->outputRectFromWidgetRect(widgetRect);
}

} // namespace PaintField
