#include "colorpanel.h"
#include "core/application.h"
#include "colorpanelcontroller.h"

namespace PaintField
{

ColorPanelController::ColorPanelController(WorkspaceController *controller, QObject *parent) :
	PanelController(parent),
	_workspace(controller)
{
	setObjectName("paintfield.panel.color");
}

QWidget *ColorPanelController::createView(QWidget *parent)
{
	ColorPanel *panel = new ColorPanel(parent);
	connect(_workspace->paletteManager(), SIGNAL(colorChanged(int,Malachite::Color)), panel, SLOT(setColor(int,Malachite::Color)));
	connect(_workspace->paletteManager(), SIGNAL(currentIndexChanged(int)), panel, SLOT(setCurrentIndex(int)));
	
	return panel;
}

}
