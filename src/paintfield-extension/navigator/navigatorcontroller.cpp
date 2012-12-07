#include "paintfield-core/canvascontroller.h"
#include "navigatorview.h"

#include "navigatorcontroller.h"

namespace PaintField {

NavigatorController::NavigatorController(CanvasController *canvas, QObject *parent) :
    QObject(parent),
    _view(new NavigatorView)
{
	if (canvas)
	{
		connectMutual(_view, SIGNAL(scaleChanged(double)), canvas->view(), SLOT(setScale(double)));
		connectMutual(_view, SIGNAL(rotationChanged(double)), canvas->view(), SLOT(setRotation(double)));
		connectMutual(_view, SIGNAL(translationChanged(QPoint)), canvas->view(), SLOT(setTranslation(QPoint)));
	}
	else
	{
		_view->setEnabled(false);
	}
}

} // namespace PaintField
