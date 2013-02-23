#include <QPainter>
#include <Malachite/Image>
#include <Malachite/Surface>
#include <Malachite/Affine2D>
#include <QPaintEvent>
#include "drawutil.h"
#include "widgets/vanishingscrollbar.h"

#include "canvasviewportsoftware.h"

using namespace Malachite;

namespace PaintField {

struct CanvasViewportSoftware::Data
{
	QTransform transformFromScene, transformToScene;
	bool transformTranslatingOnly = true;
	
	SurfaceU8 surface;
	QSize size;
	QPointSet tileKeys;
	
	QVector<QRect> accurateUpdateSceneRects;
	QRect unionAccurateUpdateSceneRect;
	bool accurateUpdateConsiderBorder = false;
	
	void clearAccurateUpdateQueue()
	{
		accurateUpdateSceneRects.clear();
		accurateUpdateConsiderBorder = false;
	}
	
	QPixmap roughUpdatePixmap;
	QPointSet roughUpdatePixelUnpaintedTiles;
	
	QHash<int, VanishingScrollBar *> scrollBars;
	
	void paintUnpaintedTilesToPixmap()
	{
		if (roughUpdatePixelUnpaintedTiles.isEmpty())
			return;
		
		QPainter painter(&roughUpdatePixmap);
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		
		for (auto key : roughUpdatePixelUnpaintedTiles)
		{
			painter.drawImage( key * Surface::tileWidth(), surface.tileRef(key).wrapInQImage() );
		}
		
		roughUpdatePixelUnpaintedTiles.clear();
	}
};

CanvasViewportSoftware::CanvasViewportSoftware(QWidget *parent) :
    QWidget(parent),
    d(new Data)
{
	d->scrollBars[Qt::Horizontal] = new VanishingScrollBar(Qt::Horizontal, this);
	d->scrollBars[Qt::Vertical] = new VanishingScrollBar(Qt::Vertical, this);
	
	connect(d->scrollBars[Qt::Horizontal], SIGNAL(valueChanged(int)), this, SIGNAL(scrollBarXChanged(int)));
	connect(d->scrollBars[Qt::Vertical], SIGNAL(valueChanged(int)), this, SIGNAL(scrollBarYChanged(int)));
	
	setAttribute(Qt::WA_TransparentForMouseEvents);
}

CanvasViewportSoftware::~CanvasViewportSoftware()
{
	delete d;
}

void CanvasViewportSoftware::setScrollBarValue(Qt::Orientation orientation, int value)
{
	Q_ASSERT(d->scrollBars.contains(orientation));
	d->scrollBars[orientation]->setValue(value);
}

void CanvasViewportSoftware::setScrollBarRange(Qt::Orientation orientation, int max, int min)
{
	Q_ASSERT(d->scrollBars.contains(orientation));
	d->scrollBars[orientation]->setRange(max, min);
}

void CanvasViewportSoftware::setScrollBarPageStep(Qt::Orientation orientation, int value)
{
	Q_ASSERT(d->scrollBars.contains(orientation));
	d->scrollBars[orientation]->setPageStep(value);
}

void CanvasViewportSoftware::setDocumentSize(const QSize &size)
{
	d->size = size;
	d->tileKeys = Surface::rectToKeys(QRect(QPoint(), size));
	d->roughUpdatePixmap = QPixmap(size);
}

void CanvasViewportSoftware::setTransform(const Malachite::Affine2D &transform, bool hasTranslation, bool hasScaling, bool hasRotation)
{
	Q_UNUSED(hasTranslation)
	d->transformFromScene = transform.toQTransform();
	d->transformToScene = transform.inverted().toQTransform();
	d->transformTranslatingOnly = !hasScaling && !hasRotation;
}

void CanvasViewportSoftware::updateTile(const QPoint &tileKey, const Image &image, const QPoint &offset)
{
	d->roughUpdatePixelUnpaintedTiles << tileKey;
	
	QPoint pos = tileKey * Surface::tileWidth() + offset;
	
	ImageU8 imageU8 = image.toImageU8();
	
	d->surface.tileRef(tileKey).paste(imageU8, offset);
	
	QRect viewRect = d->transformFromScene.mapRect(QRectF(pos, imageU8.size())).toAlignedRect() & QRect(QPoint(), size());
	
	if (viewRect.isEmpty())
		return;
	
	d->accurateUpdateSceneRects << viewRect;
	
	int tileRight = (d->size.width() - 1) / Surface::tileWidth();
	int tileBottom = (d->size.height() - 1) / Surface::tileWidth();
	
	d->accurateUpdateConsiderBorder |= (tileKey.x() <= 0 || tileKey.x() >= tileRight || tileKey.y() <= 0 || tileKey.y() >= tileBottom);
	
	//repaint(viewRect);
}

void CanvasViewportSoftware::afterUpdateTile()
{
	QRect unionRect;
	for (auto rect : d->accurateUpdateSceneRects)
	{
		unionRect |= rect;
	}
	
	d->unionAccurateUpdateSceneRect = unionRect;
	
#ifdef PAINTFIELD_COREGRAPHICS_REPAINT
	if (unionRect.width() <= Surface::tileWidth() && unionRect.height() <= Surface::tileWidth())
	{
		d->accurateUpdateSceneRects = { unionRect };
		repaint(unionRect);
	}
	else
	{
		for (auto rect : d->accurateUpdateSceneRects)
			repaint(rect);
	}
	
#else
	if (unionRect.width() <= Surface::tileWidth() && unionRect.height() <= Surface::tileWidth())
	{
		d->accurateUpdateSceneRects = { unionRect };
		repaint(unionRect);
		d->clearAccurateUpdateQueue();
	}
	else
	{
		auto rects = d->accurateUpdateSceneRects;
		
		for (auto rect : rects)
		{
			d->accurateUpdateSceneRects = { rect };
			repaint(rect);
			d->accurateUpdateSceneRects.clear();
		}
		d->accurateUpdateConsiderBorder = false;
	}
#endif
}

void CanvasViewportSoftware::updateAccurately()
{
	QVector<QRect> rects;
	
	auto sceneRect = QRect(QPoint(), d->size);
	auto viewRectOnScene = d->transformToScene.mapRect(QRectF(QPoint(), this->size())).toAlignedRect();
	
	auto rect = sceneRect & viewRectOnScene;
	
	for (const QPoint &key : Surface::rectToKeys(rect))
		rects << d->transformFromScene.mapRect( QRectF( Surface::keyToRect( key ) ) ).toAlignedRect();
	
	d->accurateUpdateSceneRects = rects;
	d->accurateUpdateConsiderBorder = true;
	repaint();
#ifndef PAINTFIELD_COREGRAPHICS_REPAINT
	d->clearAccurateUpdateQueue();
#endif
}

void CanvasViewportSoftware::repaintRects(const QVector<QRect> &rects, bool considerBorder)
{
	QPainter painter(this);
	if (!considerBorder)
		painter.setCompositionMode(QPainter::CompositionMode_Source);
	
	for (const QRect &rect : rects)
	{
		if (rect.isEmpty())
			continue;
		
		QRect requiredSceneRect = d->transformToScene.mapRect(QRectF(rect)).toAlignedRect();
		
		if ( d->transformTranslatingOnly )
		{
			auto image = d->surface.crop<ImageU8>( requiredSceneRect );
			
			painter.setTransform( d->transformFromScene );
			
			if ( considerBorder )
				painter.setClipRect( QRect( QPoint(), d->size ) );
			
			painter.drawImage( requiredSceneRect.topLeft(), image.wrapInQImage() );
		}
		else
		{
			requiredSceneRect.adjust(-1, -1, 1, 1);
			
			auto image = d->surface.crop<ImageU8>( requiredSceneRect );
			
			QImage viewImage( rect.size(), QImage::Format_ARGB32_Premultiplied );
			viewImage.fill( 0 );
			auto viewImageOffset = rect.topLeft();
			
			{
				QPainter imagePainter( &viewImage );
				imagePainter.setRenderHint( QPainter::SmoothPixmapTransform, true );
				imagePainter.setTransform( d->transformFromScene * QTransform::fromTranslate( -viewImageOffset.x(), -viewImageOffset.y() ) );
				imagePainter.drawImage( requiredSceneRect.topLeft(), image.wrapInQImage() );
			}
			
			if ( considerBorder )
			{
				QPainterPath path;
				path.addRect( QRect( QPoint(), d->size ) );
				painter.setClipPath( path * d->transformFromScene );
			}
			
			painter.drawImage( viewImageOffset, viewImage );
		}
	}
}

void CanvasViewportSoftware::paintEvent(QPaintEvent *ev)
{
	QPainter painter(this);
	
#ifdef PAINTFIELD_COREGRAPHICS_REPAINT
	// In Mac, sometimes the repaint event is merged with another update event and the repaint rect is expanded
	if (!d->unionAccurateUpdateSceneRect.contains(ev->rect()))
		d->clearAccurateUpdateQueue();
#endif
	
	if (d->accurateUpdateSceneRects.isEmpty()) // rough update, painting whole event rect
	{
		PAINTFIELD_DEBUG << "painting roughly";
		
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		
		painter.setTransform(d->transformFromScene);
		d->paintUnpaintedTilesToPixmap();
		painter.drawPixmap(QPoint(), d->roughUpdatePixmap);
	}
	else // accurate update, painting designated rects only
	{
		PAINTFIELD_DEBUG << "painting accurately";
		
		repaintRects(d->accurateUpdateSceneRects, d->accurateUpdateConsiderBorder);
		
		// In Core Graphics graphics engine, multiple repaint calls in one event loop seem to be put together
#ifdef PAINTFIELD_COREGRAPHICS_REPAINT
		d->clearAccurateUpdateQueue();
#endif
	}
}

void CanvasViewportSoftware::resizeEvent(QResizeEvent *)
{
	int barWidthX = d->scrollBars[Qt::Horizontal]->totalBarWidth();
	int barWidthY = d->scrollBars[Qt::Vertical]->totalBarWidth();
	
	auto widgetRect = QRect(QPoint(), geometry().size());
	
	auto scrollBarXRect = widgetRect.adjusted(0, widgetRect.height() - barWidthY, -barWidthX, 0);
	auto scrollBarYRect = widgetRect.adjusted(widgetRect.width() - barWidthX, 0, 0, -barWidthY);
	
	d->scrollBars[Qt::Horizontal]->setGeometry(scrollBarXRect);
	d->scrollBars[Qt::Vertical]->setGeometry(scrollBarYRect);
}

}
