#include <Malachite/Image>
#include <Malachite/Affine2D>
#include <Malachite/Surface>
#include <QWidget>
#include <QPainter>
#include <QResizeEvent>

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
	QTransform transformFromScene;
	bool translatingOnly;
	QPoint translationToScene;
}
@end

@implementation PaintField_CanvasCocoaViewport
	
	-(BOOL)wantsDefaultClipping
	{
		return NO;
	}
	
	-(BOOL)isOpaque
	{
		return YES;
	}
	
	- (void)drawRect:(NSRect)dirtyRect
	{
		auto height = [self frame].size.height;
		auto flipRect = [height](const QRect &rect) { return flippedRect(rect, height); };
		
		auto cocoaViewRect = qRectFromCGRect(dirtyRect);
		auto viewRect = flipRect(cocoaViewRect);
		
		auto context = static_cast<CGContextRef>([[NSGraphicsContext currentContext] graphicsPort]);
		CGContextSetBlendMode(context, kCGBlendModeCopy);
		
		if (translatingOnly) // easy, view is only translated
		{
			auto drawInViewRect = [&](const QRect &viewRect)
			{
				// obtain image to draw
				auto sceneRect = viewRect.translated(translationToScene);
				
				auto image = surface->crop<ImageU8>(sceneRect);
				
				// convert image into CGImageRef
				auto pixmap = QPixmap::fromImage(image.wrapInQImage());
				auto cgImage = pixmap.toMacCGImageRef();
				
				// draw
				CGContextDrawImage(context, cgRectFromQRect(flipRect(viewRect)), cgImage);
			};
			
			drawDivided(viewRect, drawInViewRect);
		}
		else
		{
			auto drawInViewRect = [&](const QRect &viewRect)
			{
				// obtain image to draw
				auto sceneRect = transformToScene.mapRect(viewRect);
				auto croppedImage = surface->crop<ImageU8>(sceneRect);
				
				QImage image(viewRect.size(), QImage::Format_ARGB32_Premultiplied);
				{
					QPainter imagePainter(&image);
					imagePainter.setRenderHint(QPainter::SmoothPixmapTransform);
					imagePainter.setTransform( transformFromScene * QTransform::fromTranslate(-viewRect.left(), -viewRect.top()) );
					imagePainter.drawImage(sceneRect.topLeft(), croppedImage.wrapInQImage());
				}
				
				// convert image into CGImageRef
				auto pixmap = QPixmap::fromImage(image);
				auto cgImage = pixmap.toMacCGImageRef();
				
				// draw
				CGContextDrawImage(context, cgRectFromQRect(flipRect(viewRect)), cgImage);
			};
			
			drawDivided(viewRect, drawInViewRect);
		}
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
		transformFromScene = toView;
	}
	
	- (void)setTranslationOnly:(bool)only translation:(const QPoint &)offsetToScene
	{
		translatingOnly = only;
		translationToScene = offsetToScene;
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
	[vp setNeedsDisplayInRect:cgRectFromQRect(flippedRect(viewRect, height))];
#else
	vp->repaint(viewRect);
#endif
}

void CanvasViewportController::placeViewport(QWidget *window)
{
	auto vp = d->viewport;
	
#ifdef PF_CANVAS_VIEWPORT_COCOA
	
	auto view = reinterpret_cast<NSView *>(window->winId());
	[view addSubview:vp positioned:NSWindowBelow relativeTo:nil];
	
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
	auto vp = d->viewport;
	d->transformToView = fromScene.toQTransform();
	d->transformToScene = toScene.toQTransform();
	
	auto translationOnly = (d->transformToScene.type() <= QTransform::TxTranslate);
	auto translationToScene = QPointF(toScene.dx(), toScene.dy()).toPoint();
	
#ifdef PF_CANVAS_VIEWPORT_COCOA
	[vp setTransformToScene:d->transformToScene toView:d->transformToView];
	[vp setTranslationOnly:translationOnly translation:translationToScene];
#else
	vp->setTransform(d->transformToScene, d->transformToView);
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
