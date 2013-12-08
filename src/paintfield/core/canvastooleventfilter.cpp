#include <QMouseEvent>
#include <QTabletEvent>
#include <QPointer>
#include <Malachite/Vec2D>
#include "canvascursorevent.h"
#include "tool.h"
#include "canvas.h"
#include "application.h"
#include "appcontroller.h"
#include "util.h"

#include "canvastooleventfilter.h"

using namespace Malachite;

namespace PaintField {

struct CanvasToolEventFilter::Data
{
	QPointer<Tool> tool;
	Canvas *canvas;
	SP<const CanvasTransforms> transforms;
	
	bool retinaMode = false;
	double mousePressure = 0;
	bool tabletOnProximity = false;

	bool sendCanvasTabletEvent(QMouseEvent *mouseEvent);
	bool sendCanvasTabletEvent(QTabletEvent *event);
};

CanvasToolEventFilter::CanvasToolEventFilter(Canvas *canvas, QObject *parent) :
	QObject(parent),
	d(new Data)
{
	d->canvas = canvas;
	d->transforms = d->canvas->transforms();

	connect(canvas, &Canvas::retinaModeChanged, this, [this](bool x){
		d->retinaMode = x;
	});
	d->retinaMode = canvas->isRetinaMode();

	connect(appController()->app(), &Application::tabletActivated, this, [this](){
		d->tabletOnProximity = true;
	});

	connect(appController()->app(), &Application::tabletDeactivated, this, [this](){
		d->tabletOnProximity = false;
	});
}

CanvasToolEventFilter::~CanvasToolEventFilter()
{
}

void CanvasToolEventFilter::setTool(Tool *tool)
{
	d->tool = tool;
}

bool CanvasToolEventFilter::eventFilter(QObject *, QEvent *event)
{
	switch (event->type()) {
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		case QEvent::MouseMove:
			event->setAccepted(d->sendCanvasTabletEvent(static_cast<QMouseEvent *>(event)));
			return event->isAccepted();
		case QEvent::TabletPress:
		case QEvent::TabletRelease:
		case QEvent::TabletMove:
			event->setAccepted(d->sendCanvasTabletEvent(static_cast<QTabletEvent *>(event)));
			return event->isAccepted();
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
			if (d->tool)
				d->tool->toolEvent(event);
			return event->isAccepted();
		default:
			return false;
	}
}

bool CanvasToolEventFilter::Data::sendCanvasTabletEvent(QTabletEvent *event)
{
	if (!this->tool)
		return false;

	auto toCanvasEventType = [](QEvent::Type type)
	{
		switch (type)
		{
			default:
			case QEvent::TabletMove:
				return EventCanvasCursorMove;
			case QEvent::TabletPress:
				return EventCanvasCursorPress;
			case QEvent::TabletRelease:
				return EventCanvasCursorRelease;
		}
	};

	TabletInputData data(event->posF() * this->transforms->windowToScene, event->pressure(), event->rotation(), event->tangentialPressure(), Vec2D(event->xTilt(), event->yTilt()));
	CanvasCursorEvent canvasEvent(toCanvasEventType(event->type()), event->globalPosF(), event->globalPos(), event->posF(), event->pos(), data, event->modifiers());

	this->tool->toolEvent(&canvasEvent);

	return canvasEvent.isAccepted();
}

bool CanvasToolEventFilter::Data::sendCanvasTabletEvent(QMouseEvent *mouseEvent)
{
	if (!this->tool)
		return false;

	if (this->tabletOnProximity)
		return true;

	auto toCanvasEventType = [](QEvent::Type type)
	{
		switch (type)
		{
			default:
			case QEvent::MouseMove:
				return EventCanvasCursorMove;
			case QEvent::MouseButtonPress:
				return EventCanvasCursorPress;
			case QEvent::MouseButtonRelease:
				return EventCanvasCursorRelease;
		}
	};
	
	int type = toCanvasEventType(mouseEvent->type());
	
	if (type == EventCanvasCursorPress)
		this->mousePressure = 1.0;
	if (type == EventCanvasCursorRelease)
		this->mousePressure = 0.0;
	
	auto pos = mouseEvent->pos();
	auto posF = Vec2D(pos);
	
	TabletInputData data(posF * this->transforms->windowToScene, this->mousePressure, 0, 0, Vec2D(0));
	CanvasCursorEvent tabletEvent(type, mouseEvent->globalPos(), mouseEvent->globalPos(), posF, pos, data, mouseEvent->modifiers());
	this->tool->toolEvent(&tabletEvent);
	return tabletEvent.isAccepted();
}

} // namespace PaintField
