#include "core/application.h"
#include "core/workspacecontroller.h"

#include "layeractioncontroller.h"

#include "colorsidebarfactory.h"
#include "layertreesidebarfactory.h"
#include "toolsidebarfactory.h"

#include "tools/brush/brushtool.h"
#include "tools/move/layermovetool.h"

#include "extensionmodule.h"

namespace PaintField
{

ExtensionModule::ExtensionModule(QObject *parent) :
	Module(parent)
{
}

void ExtensionModule::initialize()
{
	app()->addSidebarFactory(new ColorSidebarFactory);
	app()->addSidebarFactory(new LayerTreeSidebarFactory);
	app()->addSidebarFactory(new ToolSidebarFactory);
	
	app()->addToolFactory(new BrushToolFactory);
	app()->addToolFactory(new LayerMoveToolFactory);
	
	connect(app()->workspaceManager(), SIGNAL(workspaceAdded(WorkspaceController*)), this, SLOT(onWorkspaceAdded(WorkspaceController*)));
}

void ExtensionModule::onWorkspaceAdded(WorkspaceController *workspace)
{
	connect(workspace, SIGNAL(canvasAboutToBeAdded(CanvasController*)), this, SLOT(onCanvasAdded(CanvasController*)));
}

void ExtensionModule::onCanvasAdded(CanvasController *canvas)
{
	new LayerActionController(canvas);
}

}


