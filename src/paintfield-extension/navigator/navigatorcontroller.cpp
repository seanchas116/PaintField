#include "paintfield-core/canvasview.h"
#include "navigatorview.h"

#include "navigatorcontroller.h"

namespace PaintField {

NavigatorController::NavigatorController(CanvasView *canvasView, QObject *parent) :
    QObject(parent),
    _canvasView(canvasView)
{}

QWidget *NavigatorController::createView()
{
	auto view = new NavigatorView;
	
	connectMutual(view, SIGNAL(scaleChanged(double)), _canvasView, SLOT(setScale(double)));
	connectMutual(view, SIGNAL(rotationChanged(double)), _canvasView, SLOT(setRotation(double)));
	connectMutual(view, SIGNAL(translationChanged(QPoint)), _canvasView, SLOT(setTranslation(QPoint)));
	
	return view;
}

} // namespace PaintField
