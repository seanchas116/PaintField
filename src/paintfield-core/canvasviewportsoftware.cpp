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
	
	QList<QRect> partialUpdateSceneRects;
	bool partialUpdateConsiderBorder = false;
	
	QHash<int, VanishingScrollBar *> scrollBars;
};

CanvasViewportSoftware::CanvasViewportSoftware(QWidget *parent) :
    QWidget(parent),
    d(new Data)
{
	d->scrollBars[Qt::Horizontal] = new VanishingScrollBar(Qt::Horizontal, this);
	d->scrollBars[Qt::Vertical] = new VanishingScrollBar(Qt::Vertical, this);
	
	connect(d->scrollBars[Qt::Horizontal], SIGNAL(valueChanged(int)), this, SIGNAL(scrollBarXChanged(int)));
	connect(d->scrollBars[Qt::Vertical], SIGNAL(valueChanged(int)), this, SIGNAL(scrollBarYChanged(int)));
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
}

void CanvasViewportSoftware::setTransform(const Malachite::Affine2D &transform, bool hasTranslation, bool hasScaling, bool hasRotation)
{
	d->transformFromScene = transform.toQTransform();
	d->transformToScene = transform.inverted().toQTransform();
	d->transformTranslatingOnly = !hasScaling && !hasRotation;
}

void CanvasViewportSoftware::updateTile(const QPoint &tileKey, const Image &image, const QPoint &offset)
{
	QPoint pos = tileKey * Surface::tileWidth() + offset;
	
	ImageU8 imageU8 = image.toImageU8();
	
	d->surface.tileRef(tileKey).paste(imageU8, offset);
	
	QRect viewRect = d->transformFromScene.mapRect(QRectF(pos, imageU8.size())).toAlignedRect();
	
	d->partialUpdateSceneRects << viewRect;
	
	int tileRight = (d->size.width() - 1) / Surface::tileWidth();
	int tileBottom = (d->size.height() - 1) / Surface::tileWidth();
	
	d->partialUpdateConsiderBorder |= (tileKey.x() <= 0 || tileKey.x() >= tileRight || tileKey.y() <= 0 || tileKey.y() >= tileBottom);
	
	repaint(viewRect);
}

void CanvasViewportSoftware::repaintRects(const QList<QRect> &rects, bool considerBorder)
{
	QPainter painter(this);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	
	for (const QRect &rect : rects)
	{
		QRect requiredSceneRect = d->transformToScene.mapRect(QRectF(rect)).toAlignedRect();
		
		auto image = d->surface.crop<ImageU8>( requiredSceneRect );
		
		if ( d->transformTranslatingOnly )
		{
			painter.setTransform( d->transformFromScene );
			
			if ( considerBorder )
				painter.setClipRect( QRect( QPoint(), d->size ) );
			
			painter.drawImage( requiredSceneRect.topLeft(), image.wrapInQImage() );
		}
		else
		{
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

void CanvasViewportSoftware::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	
	if (d->partialUpdateSceneRects.isEmpty())
	{
		QList<QRect> rects;
		
		for (const QPoint &key : Surface::rectToKeys(QRect(QPoint(), d->size)))
			rects << d->transformFromScene.mapRect( QRectF( Surface::keyToRect( key ) ) ).toAlignedRect();
		
		repaintRects(rects, true);
	}
	else
	{
		repaintRects(d->partialUpdateSceneRects, d->partialUpdateConsiderBorder);
		
		d->partialUpdateConsiderBorder = false;
		d->partialUpdateSceneRects.clear();
	}
	
	/*
	if (!d->partialUpdateSceneRect.isEmpty())
	{
		QRect requiredSceneRect = d->transformToScene.mapRect(QRectF(d->partialUpdateSceneRect)).toAlignedRect();
		
		auto image = d->surface.crop<ImageU8>( requiredSceneRect );
		
		if ( d->transformTranslatingOnly )
		{
			PAINTFIELD_DEBUG << "translation only";
			
			painter.setTransform( d->transformFromScene );
			
			if ( d->partialUpdateBorder )
				painter.setClipRect( QRect( QPoint(), d->size ) );
			
			painter.drawImage( requiredSceneRect.topLeft(), image.wrapInQImage() );
		}
		else
		{
			QImage viewImage( d->partialUpdateSceneRect.size(), QImage::Format_ARGB32_Premultiplied );
			viewImage.fill( 0 );
			auto viewImageOffset = d->partialUpdateSceneRect.topLeft();
			
			{
				QPainter imagePainter( &viewImage );
				imagePainter.setRenderHint( QPainter::SmoothPixmapTransform, true );
				imagePainter.setTransform( d->transformFromScene * QTransform::fromTranslate( -viewImageOffset.x(), -viewImageOffset.y() ) );
				imagePainter.drawImage( requiredSceneRect.topLeft(), image.wrapInQImage() );
			}
			
			if (d->partialUpdateBorder)
			{
				QPainterPath path;
				path.addRect( QRect( QPoint(), d->size ) );
				painter.setClipPath( path * d->transformFromScene );
			}
			
			painter.drawImage( viewImageOffset, viewImage );
		}
		
		d->partialUpdateSceneRect = QRect();
		d->partialUpdateBorder = false;
	}
	else
	{
		painter.setTransform(d->transformFromScene);
		
		d->pasteUnpastedTilesToPixmap();
		
		painter.drawPixmap(QPoint(), d->pixmap);
	}*/
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
