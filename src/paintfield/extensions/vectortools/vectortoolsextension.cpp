#include <QtPlugin>

#include "paintfield/core/settingsmanager.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/widgets/simplebutton.h"
#include "paintfield/core/rectlayer.h"

#include "shapesidebar.h"
#include "fillstrokesidebar.h"
#include "recttool.h"
#include "vectortoolsextension.h"

namespace PaintField {

static const QString fillStrokeSideBarId = "paintfield.sidebar.fillStroke";
static const QString shapeSideBarId = "paintfield.sidebar.shape";

VectorToolsExtension::VectorToolsExtension(Canvas *canvas, QObject *parent) :
	CanvasExtension(canvas, parent)
{
	addSideBar(fillStrokeSideBarId, new FillStrokeSideBar(canvas ? canvas->workspace() : 0, canvas ? canvas->document()->layerScene() : 0));
	addSideBar(shapeSideBarId, canvas ? new ShapeSideBar(canvas->document()->layerScene()) : new QWidget());
}

static const QString rectToolId = "paintfield.tool.rectangle";
static const QString ellipseToolId = "paintfield.tool.ellipse";
static const QString textToolId = "paintfield.tool.text";

Tool *VectorToolsExtension::createTool(const QString &name, Canvas *canvas)
{
	if (name == rectToolId)
		return new RectTool(RectTool::AddRect, canvas);
	
	if (name == ellipseToolId)
		return new RectTool(RectTool::AddEllipse, canvas);
	
	if (name == textToolId)
		return new RectTool(RectTool::AddText, canvas);
	
	return 0;
}

void VectorToolsExtensionFactory::initialize(AppController *app)
{
	{
		auto text = tr("Rectangle");
		auto icon = SimpleButton::createIcon(":/icons/24x24/rect.svg");
		app->settingsManager()->declareTool(rectToolId, ToolInfo(text, icon, QStringList()));
	}
	
	{
		auto text = tr("Ellipse");
		auto icon = SimpleButton::createIcon(":/icons/24x24/ellipse.svg");
		app->settingsManager()->declareTool(ellipseToolId, ToolInfo(text, icon, QStringList()));
	}
	
	{
		auto text = tr("Text");
		auto icon = SimpleButton::createIcon(":/icons/24x24/text.svg");
		app->settingsManager()->declareTool(textToolId, ToolInfo(text, icon, QStringList()));
	}
	
	{
		app->settingsManager()->declareSideBar(fillStrokeSideBarId, SideBarInfo(tr("Fill and Stroke")));
		app->settingsManager()->declareSideBar(shapeSideBarId, SideBarInfo(tr("Shape")));
	}
}

} // namespace PaintField

Q_EXPORT_PLUGIN2(paintfield-vectortools, PaintField::VectorToolsExtensionFactory)
