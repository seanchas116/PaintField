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
	
	QPixmap pixmap;
	QPointSet keysUnpastedToPixmap;
	
	QRect partialUpdateSceneRect;
	bool partialUpdateBorder = false;
	
	void pasteUnpastedTilesToPixmap()
	{
		QPainter pixmapPainter(&pixmap);
		pixmapPainter.setCompositionMode(QPainter::CompositionMode_Source);
		
		for (auto key : keysUnpastedToPixmap)
		{
			QPoint pos = key * Surface::tileWidth();
			pixmapPainter.drawImage(pos, surface.tile(key).wrapInQImage());
		}
		
		keysUnpastedToPixmap.clear();
	}
	
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
	d->pixmap = QPixmap(size);
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
	d->keysUnpastedToPixmap << tileKey;
	
	QRect viewRect = d->transformFromScene.mapRect(QRectF(pos, imageU8.size())).toAlignedRect();
	
	d->partialUpdateSceneRect |= viewRect;
	
	int tileRight = (d->size.width() - 1) / Surface::tileWidth();
	int tileBottom = (d->size.height() - 1) / Surface::tileWidth();
	
	d->partialUpdateBorder |= (tileKey.x() <= 0 || tileKey.x() >= tileRight || tileKey.y() <= 0 || tileKey.y() >= tileBottom);
	
	repaint(viewRect);
}

void CanvasViewportSoftware::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setTransform(d->transformFromScene);
	//painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	
	if (!d->partialUpdateSceneRect.isEmpty())
	{
		if (d->partialUpdateBorder)
			painter.setClipRect(QRect(QPoint(), d->size));
		
		QRect requiredSceneRect = d->transformToScene.mapRect(QRectF(d->partialUpdateSceneRect)).toAlignedRect();
		requiredSceneRect.adjust(-1, -1, 1, 1);
		
		auto image = d->surface.crop<ImageU8>(requiredSceneRect);
		painter.drawImage(requiredSceneRect.topLeft(), image.wrapInQImage());
		
		d->partialUpdateSceneRect = QRect();
		d->partialUpdateBorder = false;
	}
	else
	{
		d->pasteUnpastedTilesToPixmap();
		
		painter.drawPixmap(QPoint(), d->pixmap);
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
