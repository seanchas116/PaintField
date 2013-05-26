#include <Malachite/Image>
#include <Malachite/Affine2D>
#include <Malachite/Surface>
#include <QWidget>
#include <QResizeEvent>

#if defined(Q_OS_MAC) && !defined(PF_FORCE_RASTER_ENGINE)
#define PF_CANVAS_VIEWPORT_COCOA
#endif

#ifdef PF_CANVAS_VIEWPORT_COCOA
#else
#include "canvasviewportnormal.h"
#endif

#include "canvasviewportcontroller.h"

using namespace Malachite;
using namespace PaintField;

#ifdef PF_CANVAS_VIEWPORT_COCOA

static inline CGRect cgRectFromQRect(const QRect &rect)
{
	return CGRectMake(rect.left(), rect.top(), rect.width(), rect.height());
}

static inline QRect qRectFromCGRect(const CGRect &rect)
{
	return QRect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}

static QRect flippedRect(const QRect &rect, int height)
{
	return QRect(rect.left(), height - rect.top() - rect.height(), rect.width(), rect.height());
}

@interface PaintField_CanvasCocoaViewport : NSView
{
	SurfaceU8 *surface;
	QTransform transformToScene;
}
@end

@implementation PaintField_CanvasCocoaViewport
	
	- (void)drawRect:(NSRect)dirtyRect
	{
		PAINTFIELD_DEBUG << "dirty rect" << qRectFromCGRect(dirtyRect);
		
		auto height = [self frame].size.height;
		
		auto cocoaViewRect = qRectFromCGRect(dirtyRect);
		auto viewRect = flippedRect(cocoaViewRect, height);
		
		auto context = static_cast<CGContextRef>([[NSGraphicsContext currentContext] graphicsPort]);
		CGContextSetBlendMode(context, kCGBlendModeCopy);
		
		auto drawInViewRect = [&](const QRect &viewRect)
		{
			PAINTFIELD_DEBUG << "draw in" << viewRect;
			auto sceneRect = transformToScene.mapRect(viewRect);
			auto image = surface->crop<ImageU8>(sceneRect);
			auto pixmap = QPixmap::fromImage(image.wrapInQImage());
			auto cgImage = pixmap.toMacCGImageRef();
			CGContextDrawImage(context, cgRectFromQRect(flippedRect(viewRect, height)), cgImage);
		};
		
		drawDivided(viewRect, drawInViewRect);
	}
	
	- (void)setSurface:(SurfaceU8 *)s
	{
		PAINTFIELD_DEBUG;
		surface = s;
	}
	
	- (void)setTransformToScene:(const QTransform &)toScene toView:(const QTransform &)toView
	{
		Q_UNUSED(toView);
		transformToScene = toScene;
	}
	
@end

#endif

namespace PaintField {

struct CanvasViewportController::Data
{
	QTransform transformToScene, transformToView;
	int viewScale = 1;
	QRect rectToBeRepainted;
	QVector<QRect> rects;
	int tileCount = 0;
	
	SurfaceU8 surface;
	
#ifdef PF_CANVAS_VIEWPORT_COCOA
	PaintField_CanvasCocoaViewport *viewport;
#else
	CanvasViewportNormal *viewport;
#endif
};

CanvasViewportController::CanvasViewportController(QObject *parent) :
    QObject(parent),
    d(new Data)
{
#ifdef PF_CANVAS_VIEWPORT_COCOA
	d->viewport = [[PaintField_CanvasCocoaViewport alloc] init];
	[d->viewport setSurface:&d->surface];
#else
	d->viewport = new CanvasViewportNormal(&d->surface);
#endif
}

CanvasViewportController::~CanvasViewportController()
{
#ifdef PF_CANVAS_VIEWPORT_COCOA
	[d->viewport release];
#else
	delete d->viewport;
#endif
	
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
	d->surface.tileRef(tileKey).paste(imageU8, offset);
	auto rect = QRect(tileKey * Surface::tileWidth() + offset, image.size());
	d->rectToBeRepainted |= rect;
	d->rects << rect;
	/*
#ifndef PF_CANVAS_VIEWPORT_COCOA
	if (d->tileCount == 1)
		d->viewport->setRepaintImage(imageU8);
#endif
	*/
}


void CanvasViewportController::endUpdateTile()
{
	auto vp = d->viewport;
	auto viewRect = d->transformToView.mapRect(d->rectToBeRepainted);
#ifdef PF_CANVAS_VIEWPORT_COCOA
	int height = [vp frame].size.height;
	[vp displayRectIgnoringOpacity:cgRectFromQRect(flippedRect(viewRect, height))];
#else
	vp->repaint(viewRect);
#endif
}

void CanvasViewportController::placeViewport(QWidget *window)
{
	auto vp = d->viewport;
	
#ifdef PF_CANVAS_VIEWPORT_COCOA
	
	auto view = reinterpret_cast<NSView *>(window->winId());
	[view addSubview:vp];
	
#else
	
	vp->setParent(window);
	
#endif
}

void CanvasViewportController::moveViewport(const QRect &rect, bool visible)
{
	auto vp = d->viewport;
	
#ifdef PF_CANVAS_VIEWPORT_COCOA
	
	[vp setFrame:cgRectFromQRect(rect)];
	[vp setHidden:visible ? NO : YES];
	
#else
	
	vp->setGeometry(rect);
	vp->show();
	vp->lower();
	vp->setVisible(visible);
	
#endif
}

void CanvasViewportController::setTransform(const Malachite::Affine2D &toScene, const Malachite::Affine2D &fromScene)
{
	d->transformToView = fromScene.toQTransform();
	d->transformToScene = toScene.toQTransform();
	
#ifdef PF_CANVAS_VIEWPORT_COCOA
	[d->viewport setTransformToScene:d->transformToScene toView:d->transformToView];
#else
	d->viewport->setTransform(d->transformToScene, d->transformToView);
#endif
}

void CanvasViewportController::setRetinaMode(bool mode)
{
	d->viewScale = mode ? 2 : 1;
}

void CanvasViewportController::setDocumentSize(const QSize &size)
{
	
}

void CanvasViewportController::update()
{
#ifdef PF_CANVAS_VIEWPORT_COCOA
	[d->viewport display];
#else
	d->viewport->update();
#endif
}

} // namespace PaintField
