#include "../tool/toolmodule.h"
#include "brush/brushtool.h"
#include "brush/brushsettingpanel.h"
#include "move/layermovetool.h"

#include "defaulttoolsmodule.h"

namespace PaintField
{

DefaultToolsModule::DefaultToolsModule(QObject *parent) :
    QObject(parent)
{
	setTitle("PaintField.DefaultTools");
	addDependency(MetaInfo("PaintField.Tool"));
}

void DefaultToolsModule::initialize()
{
	ToolModule::toolManager()->addToolFactory(new BrushToolFactory());
	ToolModule::toolManager()->addToolFactory(new LayerMoveTool());
	
	app()->mainWindow()->addPanel(new BrushSettingPanel());
}

}
