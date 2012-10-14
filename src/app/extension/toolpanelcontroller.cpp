#include "toolpanel.h"

#include "toolpanelcontroller.h"

namespace PaintField
{

QWidget *ToolPanelController::createView(QWidget *parent)
{
	return new ToolPanel(_workspace, parent);
}

}
