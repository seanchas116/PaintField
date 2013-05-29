#import <Cocoa/Cocoa.h>
#include <QWidget>
#include "canvasviewportutil.h"

#include "canvasviewportcocoa.h"

using namespace Malachite;
using namespace PaintField;

static inline CGRect cgRectFromQRect(const QRect &rect)
{
	return CGRectMake(rect.left(), rect.top(), rect.width(), rect.height());
}

static inline QRect qRectFromCGRect(const CGRect &rect)
{
	return QRectF(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height).toRect();
}

static inline QRect flippedRect(const QRect &rect, int height)
{
	return QRect(rect.left(), height - rect.top() - rect.height(), rect.width(), rect.height());
}

@interface PaintField_CanvasCocoaViewport : NSView
{
	CanvasViewportState *state;
	CGColorSpaceRef colorSpace;
	CGColorRef backgroundColor;
}
@end

@implementation PaintField_CanvasCocoaViewport

-(id)init
{
	self = [super init];
	if (self)
	{
		QPixmap pixmap(1,1);
		colorSpace = CGImageGetColorSpace(pixmap.toMacCGImageRef());
		CFRetain(colorSpace);
		
		CGFloat components[] = { 0.5f, 0.5f, 0.5f, 1.f };
		backgroundColor = CGColorCreate(colorSpace, components);
	}
	return self;
}

-(void)dealloc
{
	CFRelease(colorSpace);
	CFRelease(backgroundColor);
	[super dealloc];
}

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
	if (!state)
		return;
	
	auto height = [self frame].size.height;
	auto flipRect = [height](const QRect &rect) { return flippedRect(rect, height); };
	
	auto cocoaViewRect = qRectFromCGRect(dirtyRect);
	auto viewRect = flipRect(cocoaViewRect);
	
	auto context = static_cast<CGContextRef>([[NSGraphicsContext currentContext] graphicsPort]);
	CGContextSetBlendMode(context, kCGBlendModeCopy);
	CGContextSetFillColorWithColor(context, backgroundColor);
	
	auto draw = [&](const QRect &rect, const QImage &image)
	{
		auto pixmap = QPixmap::fromImage(image);
		auto cgImage = pixmap.toMacCGImageRef();
		CGContextDrawImage(context, cgRectFromQRect(flipRect(rect)), cgImage);
	};
	
	auto drawBackground = [&](const QRect &rect)
	{
		CGContextFillRect(context, cgRectFromQRect(flipRect(rect)));
	};
	
	drawViewport(viewRect, state, draw, drawBackground);
}

- (void)setState:(CanvasViewportState *)s
{
	state = s;
}

@end

namespace PaintField
{

CanvasViewportCocoaWrapper::CanvasViewportCocoaWrapper()
{
	auto vp = [[PaintField_CanvasCocoaViewport alloc] init];
	[vp setState: nullptr];
	_viewport = vp;
}

CanvasViewportCocoaWrapper::~CanvasViewportCocoaWrapper()
{
	auto vp = static_cast<PaintField_CanvasCocoaViewport *>(_viewport);
	[vp setState: nullptr];
	[vp release];
}

void CanvasViewportCocoaWrapper::setState(CanvasViewportState *state)
{
	auto vp = static_cast<PaintField_CanvasCocoaViewport *>(_viewport);
	[vp setState: state];
}

void CanvasViewportCocoaWrapper::repaint(const QRect &rect)
{
	auto vp = static_cast<PaintField_CanvasCocoaViewport *>(_viewport);
	
	int height = [vp frame].size.height;
	[vp setNeedsDisplayInRect: cgRectFromQRect(flippedRect(rect, height))];
}

void CanvasViewportCocoaWrapper::update()
{
	auto vp = static_cast<PaintField_CanvasCocoaViewport *>(_viewport);
	[vp setNeedsDisplay: YES];
}

void CanvasViewportCocoaWrapper::placeViewport(QWidget *window)
{
	auto vp = static_cast<PaintField_CanvasCocoaViewport *>(_viewport);
	
	auto view = reinterpret_cast<NSView *>(window->winId());
	[view addSubview: vp positioned: NSWindowBelow relativeTo: nil];
}

void CanvasViewportCocoaWrapper::moveViewport(const QRect &rect, bool visible)
{
	auto vp = static_cast<PaintField_CanvasCocoaViewport *>(_viewport);
	
	[vp setFrame:cgRectFromQRect(rect)];
	[vp setHidden:visible ? NO : YES];
}

}
