#include "paintfield/core/canvas.h"
#include "paintfield/core/canvasview.h"
#include "navigatorview.h"

#include "navigatorcontroller.h"

namespace PaintField {

NavigatorController::NavigatorController(Canvas *canvas, QObject *parent) :
    QObject(parent),
    _view(new NavigatorView)
{
	NavigatorView *navigatorView = _view.data();
	
	if (canvas)
	{
		connect(navigatorView, SIGNAL(scaleChanged(double)), canvas, SLOT(setScale(double)));
		connect(canvas, SIGNAL(scaleChanged(double)), navigatorView, SLOT(setScale(double)));
		
		connect(navigatorView, SIGNAL(rotationChanged(double)), canvas, SLOT(setRotation(double)));
		connect(canvas, SIGNAL(rotationChanged(double)), navigatorView, SLOT(setRotation(double)));
		
		connect(navigatorView, SIGNAL(translationChanged(QPoint)), canvas, SLOT(setTranslation(QPoint)));
		connect(canvas, SIGNAL(translationChanged(QPoint)), navigatorView, SLOT(setTranslation(QPoint)));
		
		connect(navigatorView, SIGNAL(mirroringEnabledChanged(bool)), canvas, SLOT(setMirroringEnabled(bool)));
		connect(canvas, SIGNAL(mirroringEnabledChanged(bool)), navigatorView, SLOT(setMirroringEnabled(bool)));
	}
	else
	{
		navigatorView->setEnabled(false);
	}
}

} // namespace PaintField
