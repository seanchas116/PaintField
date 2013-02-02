#include <QPainter>
#include <Malachite/Image>
#include <Malachite/Surface>
#include "drawutil.h"

#include "canvasviewportcontrollersoftware.h"

using namespace Malachite;

namespace PaintField {

struct CanvasViewportSoftware::Data
{
	QTransform transform;
	QPixmap pixmap;
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
}

void CanvasViewportSoftware::setTransform(const Affine2D &transform)
{
	d->transform = transform.toQTransform();
	PAINTFIELD_DEBUG << d->transform;
}

QTransform CanvasViewportSoftware::transform() const
{
	return d->transform;
}

QPixmap *CanvasViewportSoftware::pixmap()
{
	return &d->pixmap;
}

void CanvasViewportSoftware::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	
	painter.setTransform(d->transform);
	painter.drawPixmap(0, 0, d->pixmap);
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

void CanvasViewportControllerSoftware::setTransform(const Affine2D &transform)
{
	d->view->setTransform(transform);
}

void CanvasViewportControllerSoftware::updateTile(const QPoint &tileKey, const Image &image, const QPoint &offset)
{
	QPoint tilePos = tileKey * Surface::TileSize;
	
	{
		QPainter painter(d->view->pixmap());
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		DrawUtil::drawMLImageFast(&painter, tilePos + offset, image);
	}
	
	QRect mappedRect = d->view->transform().mapRect(QRectF(tilePos + offset, image.size())).toAlignedRect();
	PAINTFIELD_DEBUG << "repainting" << mappedRect;
	d->view->repaint(mappedRect);
}

void CanvasViewportControllerSoftware::update()
{
	
}

}
