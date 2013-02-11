#include <QPainter>
#include <Malachite/Image>
#include <Malachite/Surface>
#include <QPaintEvent>
#include "drawutil.h"

#include "canvasviewportcontrollersoftware.h"

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
};

CanvasViewportSoftware::CanvasViewportSoftware(QWidget *parent) :
    QWidget(parent),
    d(new Data)
{
}

CanvasViewportSoftware::~CanvasViewportSoftware()
{
	delete d;
}

void CanvasViewportSoftware::setDocumentSize(const QSize &size)
{
	d->pixmap = QPixmap(size);
	d->size = size;
}

void CanvasViewportSoftware::setTransform(const Affine2D &transform, bool isTranslatingOnly)
{
	d->transformFromScene = transform.toQTransform();
	d->transformToScene = transform.inverted().toQTransform();
	d->transformTranslatingOnly = isTranslatingOnly;
}

void CanvasViewportSoftware::pasteImage(const QPoint &tileKey, const Image &image, const QPoint &offset)
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

struct CanvasViewportControllerSoftware::Data
{
	CanvasViewportSoftware *view;
};

CanvasViewportControllerSoftware::CanvasViewportControllerSoftware(QObject *parent) :
    AbstractCanvasViewportController(parent),
    d(new Data)
{
	d->view = new CanvasViewportSoftware();
	emit ready();
}

CanvasViewportControllerSoftware::~CanvasViewportControllerSoftware()
{
	delete d;
}

QWidget *CanvasViewportControllerSoftware::view()
{
	return d->view;
}

void CanvasViewportControllerSoftware::setDocumentSize(const QSize &size)
{
	d->view->setDocumentSize(size);
}

void CanvasViewportControllerSoftware::setTransform(const Affine2D &transform, bool hasTranslation, bool hasScaling, bool hasRotation)
{
	Q_UNUSED(hasTranslation);
	d->view->setTransform(transform, !hasScaling && !hasRotation);
}

void CanvasViewportControllerSoftware::updateTile(const QPoint &tileKey, const Image &image, const QPoint &offset)
{
	d->view->pasteImage(tileKey, image, offset);
}

void CanvasViewportControllerSoftware::update()
{
	
}

}
