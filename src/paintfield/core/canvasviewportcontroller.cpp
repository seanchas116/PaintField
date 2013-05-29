#include <Malachite/Image>
#include <Malachite/Affine2D>
#include <Malachite/Surface>
#include <QWidget>
#include <QPainter>
#include <QResizeEvent>

#include "canvasviewportutil.h"

#include "canvasviewportcontroller.h"

#ifdef PF_CANVAS_VIEWPORT_COCOA
#include "canvasviewportcocoa.h"
#else
#include "canvasviewportnormal.h"
#endif

using namespace Malachite;

namespace PaintField {

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
	PAINTFIELD_DEBUG << tileCount;
}

void CanvasViewportController::updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset)
{
	auto imageU8 = image.toImageU8();
	d->state.surface.tileRef(tileKey).paste(imageU8, offset);
	auto rect = QRect(tileKey * Surface::tileWidth() + offset, image.size());
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
	auto viewRect = d->state.transformToView.mapRect(d->rectToBeRepainted);
	
	if (d->state.retinaMode)
		viewRect = QRectF(viewRect.left() * 0.5, viewRect.top() * 0.5, viewRect.width() * 0.5, viewRect.height() * 0.5).toAlignedRect();
	
	d->viewportWrapper.repaint(viewRect);
}

void CanvasViewportController::placeViewport(QWidget *window)
{
	d->viewportWrapper.placeViewport(window);
}

void CanvasViewportController::moveViewport(const QRect &rect, bool visible)
{
	d->viewportWrapper.moveViewport(rect, visible);
}

void CanvasViewportController::setTransform(const Malachite::Affine2D &toScene, const Malachite::Affine2D &fromScene)
{
	d->state.transformToView = fromScene.toQTransform();
	d->state.transformToScene = toScene.toQTransform();
	
	d->state.translationOnly = (d->state.transformToScene.type() <= QTransform::TxTranslate);
	d->state.translationToScene = QPointF(toScene.dx(), toScene.dy()).toPoint();
}

void CanvasViewportController::setRetinaMode(bool mode)
{
	d->state.retinaMode = mode;
}

void CanvasViewportController::setDocumentSize(const QSize &size)
{
	d->state.documentSize = size;
}

void CanvasViewportController::update()
{
	d->viewportWrapper.update();
}

} // namespace PaintField
