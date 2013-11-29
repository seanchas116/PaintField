#include "canvasview.h"

#include "canvasviewport.h"
#include "util.h"
#include "canvas.h"

namespace PaintField {

struct CanvasView::Data
{
	CanvasView *mSelf;
	Canvas *mCanvas = 0;
	QPointer<CanvasViewport> mViewport;

	void onMoved()
	{
		if (!mViewport)
			return;
		QRect geom(Util::mapToWindow(mSelf, QPoint()), mSelf->geometry().size());
		mViewport->setParent(mSelf->window());
		mViewport->setGeometry(geom);
		mViewport->setVisible(mSelf->isVisible());
		mViewport->raise();
	}
};

CanvasView::CanvasView(Canvas *canvas, QWidget *parent) :
	QWidget(parent),
	d(new Data)
{
	d->mSelf = this;
	d->mCanvas = canvas;
	d->mViewport = new CanvasViewport(canvas);
	canvas->setView(this);
	d->onMoved();
	connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)), this, SLOT(deleteLater()));
	connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)), d->mViewport, SLOT(deleteLater()));
}

CanvasView::~CanvasView()
{
	if (d->mViewport)
		delete d->mViewport.data();
}

Canvas *CanvasView::canvas()
{
	return d->mCanvas;
}

CanvasViewport *CanvasView::viewport()
{
	return d->mViewport;
}

void CanvasView::resizeEvent(QResizeEvent *)
{
	d->onMoved();
}

void CanvasView::changeEvent(QEvent *ev)
{
	switch (ev->type())
	{
		case QEvent::ParentChange:
		case QEvent::EnabledChange:
				d->onMoved();
		default:
			break;
	}
}

void CanvasView::showEvent(QShowEvent *)
{
	d->onMoved();
}

void CanvasView::hideEvent(QHideEvent *)
{
	d->onMoved();
}

} // namespace PaintField
