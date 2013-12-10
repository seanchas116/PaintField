#include "selectionextension.h"
#include "selectionpainttool.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/settingsmanager.h"
#include "paintfield/core/widgets/simplebutton.h"

namespace PaintField {

namespace {

const QString paintToolName = "paintfield.tool.selectionPaint";

}

SelectionExtension::SelectionExtension(Workspace *workspace, QObject *parent) :
	WorkspaceExtension(workspace, parent)
{

}

Tool *SelectionExtension::createTool(const QString &name, Canvas *canvas)
{
	if (name == paintToolName) {
		return new SelectionPaintTool(canvas);
	}
	return 0;
}


SelectionExtensionFactory::SelectionExtensionFactory(QObject *parent) :
	ExtensionFactory(parent)
{
}

void SelectionExtensionFactory::initialize(AppController *app)
{
	{
		auto text = tr("Selection Painting");
		auto icon = SimpleButton::createIcon(":/icons/24x24/brush.svg");
		app->settingsManager()->declareTool(paintToolName, text, icon, {});
	}
}

WorkspaceExtensionList SelectionExtensionFactory::createWorkspaceExtensions(Workspace *workspace, QObject *parent)
{
	return { new SelectionExtension(workspace, parent) };
}


} // namespace PaintField
