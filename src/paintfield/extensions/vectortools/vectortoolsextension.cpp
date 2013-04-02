#include <QtPlugin>

#include "paintfield/core/settingsmanager.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/widgets/simplebutton.h"

#include "fillstrokesidebar.h"
#include "recttool.h"
#include "vectortoolsextension.h"

namespace PaintField {

static const QString fillStrokeSideBarId = "paintfield.sidebar.fillStroke";

VectorToolsExtension::VectorToolsExtension(Canvas *canvas, QObject *parent) :
	CanvasExtension(canvas, parent)
{
	addSideBar(fillStrokeSideBarId, new FillStrokeSideBar(canvas ? canvas->workspace() : 0, canvas ? canvas->document()->layerScene() : 0));
}

static const QString rectToolId = "paintfield.tool.rectangle";

Tool *VectorToolsExtension::createTool(const QString &name, Canvas *canvas)
{
	if (name == rectToolId)
		return new RectTool(canvas);
	
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
		app->settingsManager()->declareSideBar(fillStrokeSideBarId, SideBarInfo(tr("Fill and Stroke")));
	}
}

} // namespace PaintField

Q_EXPORT_PLUGIN2(paintfield-vectortools, PaintField::VectorToolsExtensionFactory)
