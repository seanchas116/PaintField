#include "selectionextension.h"
#include "selectionpainttool.h"
#include "selectionrecttool.h"
#include "selectionbrushsidebarviewmodel.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/settingsmanager.h"
#include "paintfield/core/widgets/simplebutton.h"
#include "paintfield/core/observablevariantmap.h"


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
	mBrushState = new ObservableVariantMap(this);
	mBrushState->setValue("size", 10);
	mBrushSidebarViewModel = new SelectionBrushSidebarViewModel(mBrushState, this);
}

Tool *SelectionExtension::createTool(const QString &name, Canvas *canvas)
{
	auto createSelectionPaintTool = [&](SelectionPaintTool::Type type) {
		auto tool = new SelectionPaintTool(type, canvas);
		Property::sync(qtProperty(tool, "brushSize"), mBrushState->customProperty("size"));
		QString title;
		if (type == SelectionPaintTool::TypeBrush) {
			title = tr("Selection Painting");
		} else {
			title = tr("Selection Eraser");
		}
		mBrushSidebarViewModel->setProperty("title", title);
		return tool;
	};

	if (name == paintToolName) {
		return createSelectionPaintTool(SelectionPaintTool::TypeBrush);
	} else if (name == eraserToolName) {
		return createSelectionPaintTool(SelectionPaintTool::TypeEraser);
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
		auto icon = SimpleButton::createIcon(":/icons/24x24/selectPaint.svg");
		app->settingsManager()->declareTool(paintToolName, text, icon, {});
	}
	{
		{
			auto text = tr("Selection Eraser");
			auto icon = SimpleButton::createIcon(":/icons/24x24/selectErase.svg");
			app->settingsManager()->declareTool(eraserToolName, text, icon, {});
		}
	}
	{
		{
			auto text = tr("Rectangle Selection");
			auto icon = SimpleButton::createIcon(":/icons/24x24/selectRect.svg");
			app->settingsManager()->declareTool(rectToolName, text, icon, {});
		}
	}
	{
		{
			auto text = tr("Ellipse Selection");
			auto icon = SimpleButton::createIcon(":/icons/24x24/selectEllipse.svg");
			app->settingsManager()->declareTool(ellipseToolName, text, icon, {});
		}
	}
}

WorkspaceExtensionList SelectionExtensionFactory::createWorkspaceExtensions(Workspace *workspace, QObject *parent)
{
	return { new SelectionExtension(workspace, parent) };
}


} // namespace PaintField
