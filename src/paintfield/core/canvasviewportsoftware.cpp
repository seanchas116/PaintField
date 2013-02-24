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
	QTransform transformToView, transformToScene;
	bool transformTranslatingOnly = true;
	bool retinaMode = false;
	
	QRect viewRect;
	
	SurfaceU8 surface;
	QSize size;
	QPointSet tileKeys;
	
	QVector<QRect> accurateUpdateSceneRects;
	QRect unionAccurateUpdateSceneRect;
	bool accurateUpdateConsiderBorder = false;
	bool wholeAccurateUpdate = false;
	
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

void CanvasViewportSoftware::setTransform(const Malachite::Affine2D &transform, bool hasTranslation, bool hasScaling, bool hasRotation, bool retinaMode)
{
	Q_UNUSED(hasTranslation)
	
	d->retinaMode = retinaMode;
	
	if (retinaMode)
	{
		PAINTFIELD_DEBUG << "retina mode";
		d->transformToView = (Affine2D::fromScale(2) * transform).toQTransform();
	}
	else
	{
		d->transformToView = transform.toQTransform();
	}
	
	d->transformToScene = d->transformToView.inverted();
	d->transformTranslatingOnly = !hasScaling && !hasRotation;
	
	if (retinaMode)
		d->viewRect = QRect(0, 0, width() * 2, height() * 2);
	else
		d->viewRect = QRect(QPoint(), size());
}

void CanvasViewportSoftware::updateTile(const QPoint &tileKey, const Image &image, const QPoint &offset)
{
	d->roughUpdatePixelUnpaintedTiles << tileKey;
	
	QPoint pos = tileKey * Surface::tileWidth() + offset;
	
	ImageU8 imageU8 = image.toImageU8();
	
	d->surface.tileRef(tileKey).paste(imageU8, offset);
	
	QRect viewRect = d->transformToView.mapRect(QRectF(pos, imageU8.size())).toAlignedRect() & d->viewRect;
	
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
		
		repaintViewRect(unionRect);
	}
	else
	{
		for (auto rect : d->accurateUpdateSceneRects)
			repaintViewRect(rect);
	}
	
#else
	if (unionRect.width() <= Surface::tileWidth() && unionRect.height() <= Surface::tileWidth())
	{
		d->accurateUpdateSceneRects = { unionRect };
		repaintViewRect(unionRect);
		d->clearAccurateUpdateQueue();
	}
	else
	{
		auto rects = d->accurateUpdateSceneRects;
		
		for (auto rect : rects)
		{
			d->accurateUpdateSceneRects = { rect };
			repaintViewRect(rect);
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
	auto viewRectOnScene = d->transformToScene.mapRect(QRectF(d->viewRect)).toAlignedRect();
	
	auto rect = sceneRect & viewRectOnScene;
	
	for (const QPoint &key : Surface::rectToKeys(rect))
		rects << d->transformToView.mapRect( QRectF( Surface::keyToRect( key ) ) ).toAlignedRect();
	
	d->accurateUpdateSceneRects = rects;
	d->accurateUpdateConsiderBorder = true;
	d->wholeAccurateUpdate = true;
	repaint();
#ifndef PAINTFIELD_COREGRAPHICS_REPAINT
	d->clearAccurateUpdateQueue();
#endif
}

void CanvasViewportSoftware::repaintViewRect(const QRect &rect)
{
	if (d->retinaMode)
	{
		QRectF newRect(double(rect.left()) * 0.5, double(rect.top()) * 0.5, double(rect.width()) * 0.5, double(rect.height()) * 0.5);
		repaint(newRect.toAlignedRect());
	}
	else
	{
		repaint(rect);
	}
}

void CanvasViewportSoftware::paintRects(const QVector<QRect> &rects, bool considerBorder)
{
	QPainter painter(this);
	if (!considerBorder)
		painter.setCompositionMode(QPainter::CompositionMode_Source);
	
	if (d->retinaMode)
		painter.scale(0.5, 0.5);
	
	if (d->transformTranslatingOnly)
	{
		auto offset = QPoint(d->transformToView.dx(), d->transformToView.dy());
		
		for (const QRect &rect : rects)
		{
			if (rect.isEmpty())
				continue;
			
			QRect requiredSceneRect = d->transformToScene.mapRect(QRectF(rect)).toAlignedRect();
			
			auto image = d->surface.crop<ImageU8>( requiredSceneRect );
			
			if ( considerBorder )
				painter.setClipRect( QRect( QPoint(), d->size ) );
			
			painter.drawImage( requiredSceneRect.topLeft() + offset, image.wrapInQImage() );
		}
	}
	else
	{
		for (const QRect &rect : rects)
		{
			if (rect.isEmpty())
				continue;
			
			QRect requiredSceneRect = d->transformToScene.mapRect(QRectF(rect)).toAlignedRect();
			
			requiredSceneRect.adjust(-1, -1, 1, 1);
			
			auto image = d->surface.crop<ImageU8>( requiredSceneRect );
			
			QImage viewImage( rect.size(), QImage::Format_ARGB32_Premultiplied );
			viewImage.fill( 0 );
			auto viewImageOffset = rect.topLeft();
			
			{
				QPainter imagePainter( &viewImage );
				imagePainter.setRenderHint( QPainter::SmoothPixmapTransform, true );
				imagePainter.setTransform( d->transformToView * QTransform::fromTranslate( -viewImageOffset.x(), -viewImageOffset.y() ) );
				imagePainter.drawImage( requiredSceneRect.topLeft(), image.wrapInQImage() );
			}
			
			if ( considerBorder )
			{
				QPainterPath path;
				path.addRect( QRect( QPoint(), d->size ) );
				painter.setClipPath( path * d->transformToView );
			}
			
			painter.drawImage( viewImageOffset, viewImage );
		}
	}
}

void CanvasViewportSoftware::paintEvent(QPaintEvent *ev)
{
#ifdef PAINTFIELD_COREGRAPHICS_REPAINT
	// In Mac, sometimes the repaint event is merged with another update event and the repaint rect is expanded
	if (!d->wholeAccurateUpdate && !d->unionAccurateUpdateSceneRect.contains(ev->rect()))
		d->clearAccurateUpdateQueue();
#endif
	
	d->wholeAccurateUpdate = false;
	
	if (d->accurateUpdateSceneRects.isEmpty()) // rough update, painting whole event rect
	{
		QPainter painter(this);
		
		PAINTFIELD_DEBUG << "painting roughly";
		
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		
		if (d->retinaMode)
			painter.setTransform(d->transformToView * QTransform::fromScale(0.5, 0.5));
		else
			painter.setTransform(d->transformToView);
		
		d->paintUnpaintedTilesToPixmap();
		painter.drawPixmap(QPoint(), d->roughUpdatePixmap);
	}
	else // accurate update, painting designated rects only
	{
		PAINTFIELD_DEBUG << "painting accurately";
		
		paintRects(d->accurateUpdateSceneRects, d->accurateUpdateConsiderBorder);
		
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
