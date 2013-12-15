#include "selectionextension.h"
#include "selectionpainttool.h"
#include "selectionrecttool.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/settingsmanager.h"
#include "paintfield/core/widgets/simplebutton.h"

namespace PaintField {

namespace {

const QString paintToolName = "paintfield.tool.selectionPaint";
const QString eraserToolName = "paintfield.tool.selectionEraser";
const QString rectToolName = "paintfield.tool.selectionRect";
const QString ellipseToolName = "paintfield.tool.selectionEllipse";

}

SelectionExtension::SelectionExtension(Workspace *workspace, QObject *parent) :
	WorkspaceExtension(workspace, parent)
{

}

Tool *SelectionExtension::createTool(const QString &name, Canvas *canvas)
{
	if (name == paintToolName) {
		return new SelectionPaintTool(SelectionPaintTool::TypeBrush, canvas);
	} else if (name == eraserToolName) {
		return new SelectionPaintTool(SelectionPaintTool::TypeEraser, canvas);
	} else if (name == rectToolName) {
		return new SelectionRectTool(SelectionRectTool::TypeRect, canvas);
	} else if (name == ellipseToolName) {
		return new SelectionRectTool(SelectionRectTool::TypeEllipse, canvas);
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

	{
		{
			auto text = tr("Selection Eraser");
			auto icon = SimpleButton::createIcon(":/icons/24x24/brush.svg");
			app->settingsManager()->declareTool(eraserToolName, text, icon, {});
		}
	}
	{
		{
			auto text = tr("Rectangle Selection");
			auto icon = SimpleButton::createIcon(":/icons/24x24/rect.svg");
			app->settingsManager()->declareTool(rectToolName, text, icon, {});
		}
	}
	{
		{
			auto text = tr("Ellipse Selection");
			auto icon = SimpleButton::createIcon(":/icons/24x24/ellipse.svg");
			app->settingsManager()->declareTool(ellipseToolName, text, icon, {});
		}
	}
}

WorkspaceExtensionList SelectionExtensionFactory::createWorkspaceExtensions(Workspace *workspace, QObject *parent)
{
	return { new SelectionExtension(workspace, parent) };
}


} // namespace PaintField
