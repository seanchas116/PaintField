#include <Malachite/Image>
#include <Malachite/Affine2D>
#include <Malachite/Surface>
#include <QWidget>
#include <QPainter>
#include <QResizeEvent>

#include "canvasviewportstate.h"

#include "canvasviewportcontroller.h"

#ifdef PF_CANVAS_VIEWPORT_COCOA
#include "canvasviewportcocoa.h"
#else
#include "canvasviewportnormal.h"
#endif

using namespace Malachite;

namespace PaintField {

/*
static QRect alignedHalfRect(const QRect &rect)
{
	int xbegin = rect.left() / 2;
	int xend = (rect.left() + rect.width() + 1) / 2;
	int ybegin = rect.top() / 2;
	int yend = (rect.top() + rect.height() + 1) / 2;
	return QRect(xbegin, ybegin, xend - xbegin, yend - ybegin);
}
*/

struct CanvasViewportController::Data
{
	int viewScale = 1;
	QRect rectToBeRepainted;
	QVector<QRect> rects;
	int tileCount = 0;
	
	CanvasViewportState state;
	
#ifdef PF_CANVAS_VIEWPORT_COCOA
	CanvasViewportCocoaWrapper viewportWrapper;
#else
	CanvasViewportNormalWrapper viewportWrapper;
#endif
};

CanvasViewportController::CanvasViewportController(QObject *parent) :
    QObject(parent),
    d(new Data)
{
	d->viewportWrapper.setState(&d->state);
}

CanvasViewportController::~CanvasViewportController()
{
	delete d;
}

void CanvasViewportController::beginUpdateTile(int tileCount)
{
	d->rects.clear();
	d->rects.reserve(tileCount);
	d->tileCount = tileCount;
	d->rectToBeRepainted = QRect();
	//PAINTFIELD_DEBUG << tileCount;
}

void CanvasViewportController::updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset)
{
	auto imageU8 = image.toImageU8();
	auto localRect = QRect(offset, image.size());
	d->state.mipmap.replace(imageU8, tileKey, localRect);
	
	auto rect = localRect.translated(tileKey * Surface::tileWidth());
	d->rectToBeRepainted |= rect;
	d->rects << rect;
	
	if (d->tileCount == 1)
	{
		d->state.cacheAvailable = true;
		d->state.cacheRect = rect;
		d->state.cacheImage = imageU8;
	}
	else
	{
		d->state.cacheAvailable = false;
	}
}

void CanvasViewportController::endUpdateTile()
{
	auto viewRect = d->state.transforms->sceneToView.mapRect(d->rectToBeRepainted);
	
	if (d->state.retinaMode)
		viewRect = QRectF(viewRect.left() * 0.5, viewRect.top() * 0.5, viewRect.width() * 0.5, viewRect.height() * 0.5).toAlignedRect();
	
	d->viewportWrapper.repaint(viewRect);
}

QWidget *CanvasViewportController::viewport()
{
	return d->viewportWrapper.viewport();
}

void CanvasViewportController::placeViewport(QWidget *window)
{
	d->viewportWrapper.placeViewport(window);
}

void CanvasViewportController::moveViewport(const QRect &rect, bool visible)
{
	d->viewportWrapper.moveViewport(rect, visible);
}

CanvasViewportSurface CanvasViewportController::mergedSurface() const
{
	return d->state.mipmap.topLevelSurface();
}

void CanvasViewportController::setTransforms(const SP<const CanvasTransforms> &transforms)
{
	d->state.mipmap.setCurrentLevel(transforms->mipmapLevel);
	d->state.transforms = transforms;
	d->state.translationOnly = (transforms->mipmapScale == 1.0 && transforms->rotation == 0.0 && !transforms->mirrored);
	d->state.translationToScene = QPointF(transforms->viewToMipmap.dx(), transforms->viewToMipmap.dy()).toPoint();
}


void CanvasViewportController::setRetinaMode(bool mode)
{
	d->state.retinaMode = mode;
}

void CanvasViewportController::setDocumentSize(const QSize &size)
{
	d->state.documentSize = size;
	d->state.mipmap.setSceneSize(size);
}

void CanvasViewportController::update()
{
	d->viewportWrapper.update();
}

void CanvasViewportController::deleteViewportLater()
{
	d->viewportWrapper.deleteLater();
}

} // namespace PaintField
