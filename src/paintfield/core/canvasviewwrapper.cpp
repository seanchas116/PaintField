#include "canvas.h"
#include "canvasview.h"

#include "canvasviewwrapper.h"

namespace PaintField {

struct CanvasViewWrapper::Data
{
	QPointer<CanvasView> view;
};

CanvasViewWrapper::CanvasViewWrapper(Canvas *canvas, QWidget *parent) :
	QWidget(parent),
	d(new Data)
{
	d->view = new CanvasView(canvas);
}

CanvasViewWrapper::~CanvasViewWrapper()
{
	if (d->view)
		d->view->deleteLater();
	
	delete d;
}

void CanvasViewWrapper::resizeEvent(QResizeEvent *)
{
	moveView();
}

void CanvasViewWrapper::showEvent(QShowEvent *)
{
	d->view->show();
}

void CanvasViewWrapper::hideEvent(QHideEvent *)
{
	d->view->hide();
}

void CanvasViewWrapper::changeEvent(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::ParentChange:
			moveView();
			break;
		case QEvent::EnabledChange:
			d->view->setEnabled(isEnabled());
			break;
		default:
			break;
	}
}

void CanvasViewWrapper::moveView()
{
	d->view->setParent(window());
	QRect geom(Util::mapToWindow(this, QPoint()), this->geometry().size());
	d->view->setGeometry(geom);
	d->view->show();
	d->view->lower();
}

} // namespace PaintField
