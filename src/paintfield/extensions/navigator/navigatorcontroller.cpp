#include "paintfield/core/canvas.h"
#include "paintfield/core/canvasview.h"
#include "navigatorview.h"

#include "navigatorcontroller.h"

namespace PaintField {

NavigatorController::NavigatorController(Canvas *canvas, QObject *parent) :
    QObject(parent),
    _view(new NavigatorView)
{
	NavigatorView *view = _view.data();
	
	if (canvas)
	{
		connect(view, SIGNAL(scaleChanged(double)), canvas, SLOT(setScale(double)));
		connect(canvas, SIGNAL(scaleChanged(double)), view, SLOT(setScale(double)));
		
		connect(view, SIGNAL(rotationChanged(double)), canvas, SLOT(setRotation(double)));
		connect(canvas, SIGNAL(rotationChanged(double)), view, SLOT(setRotation(double)));
		
		connect(view, SIGNAL(translationChanged(QPoint)), canvas, SLOT(setTranslation(QPoint)));
		connect(canvas, SIGNAL(translationChanged(QPoint)), view, SLOT(setTranslation(QPoint)));
		
		connect(view, SIGNAL(mirroringEnabledChanged(bool)), canvas, SLOT(setMirrored(bool)));
		connect(canvas, SIGNAL(mirroredChanged(bool)), view, SLOT(setMirroringEnabled(bool)));
		
		connect(view, SIGNAL(retinaModeChanged(bool)), canvas, SLOT(setRetinaMode(bool)));
		connect(canvas, SIGNAL(retinaModeChanged(bool)), view, SLOT(setRetinaMode(bool)));
		
		view->setTranslation(canvas->translation());
		view->setScale(canvas->scale());
		view->setRotation(canvas->rotation());
		view->setMirroringEnabled(canvas->isMirrored());
	}
	else
	{
		view->setEnabled(false);
	}
}

} // namespace PaintField
