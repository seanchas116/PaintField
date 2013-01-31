#include "paintfield-core/canvascontroller.h"
#include "paintfield-core/canvasview.h"
#include "navigatorview.h"

#include "navigatorcontroller.h"

namespace PaintField {

NavigatorController::NavigatorController(CanvasController *canvas, QObject *parent) :
    QObject(parent),
    _view(new NavigatorView)
{
	NavigatorView *navigatorView = _view.data();
	
	if (canvas)
	{
		connect(navigatorView, SIGNAL(scaleChanged(double)), canvas->view(), SLOT(setViewScale(double)));
		connect(canvas->view(), SIGNAL(scaleChanged(double)), navigatorView, SLOT(setScale(double)));
		
		connect(navigatorView, SIGNAL(rotationChanged(double)), canvas->view(), SLOT(setViewRotation(double)));
		connect(canvas->view(), SIGNAL(rotationChanged(double)), navigatorView, SLOT(setRotation(double)));
		
		connect(navigatorView, SIGNAL(translationChanged(QPoint)), canvas->view(), SLOT(setTranslation(QPoint)));
		connect(canvas->view(), SIGNAL(translationChanged(QPoint)), navigatorView, SLOT(setTranslation(QPoint)));
		
		connect(navigatorView, SIGNAL(mirroringEnabledChanged(bool)), canvas->view(), SLOT(setMirroringEnabled(bool)));
		connect(canvas->view(), SIGNAL(mirroringEnabledChanged(bool)), navigatorView, SLOT(setMirroringEnabled(bool)));
	}
	else
	{
		navigatorView->setEnabled(false);
	}
}

} // namespace PaintField
