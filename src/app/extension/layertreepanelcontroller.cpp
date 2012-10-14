#include "core/widgets/simplebutton.h"
#include "layertreepanel.h"

#include "layertreepanelcontroller.h"

namespace PaintField
{

QWidget *LayerTreePanelController::createView(QWidget *parent)
{
	LayerTreePanel *panel = new LayerTreePanel(_canvas->document()->layerModel(), parent);
	
	QMenu *addMenu = new QMenu(panel);
	
	addMenu->addAction(_actionController->newRasterAction());
	addMenu->addAction(_actionController->newGroupAction());
	addMenu->addAction(_actionController->importAction());
	
	panel->addButton()->setMenu(addMenu);
	
	connect(panel->removeButton(), SIGNAL(pressed()), _actionController, SLOT(removeLayers()));
	
	QMenu *miscMenu = new QMenu(panel);
	miscMenu->addAction(_actionController->mergeAction());
	
	panel->miscButton()->setMenu(miscMenu);
	
	return panel;
}

}
