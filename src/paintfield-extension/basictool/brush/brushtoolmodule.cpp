#include "paintfield-core/widgets/simplebutton.h"

#include "brushtool.h"
#include "brushsettingsidebar.h"

#include "brushtoolmodule.h"

namespace PaintField
{

const QString _brushToolName("paintfield.tool.brush");
const QString _brushSidebarName("paintfield.sidebar.brush");

BrushToolModule::BrushToolModule(WorkspaceController *parent) :
	WorkspaceModule(parent)
{
}

Tool *BrushToolModule::createTool(const QString &name, CanvasView *parent)
{
	if (name == _brushToolName)
	{
		BrushTool *tool = new BrushTool(parent);
		tool->setBrushSetting(&_setting);
		return tool;
	}
	return 0;
}

QWidget *BrushToolModule::createSidebar(const QString &name)
{
	if (name == _brushSidebarName)
		return new BrushSettingSidebar(&_setting);
	return 0;
}

void BrushToolModuleFactory::initialize(Application *app)
{
	{
		QString text = QObject::tr("Brush");
		QIcon icon = SimpleButton::createSimpleIconSet(":/icons/32x32/brush.svg");
		QVector<int> supportedTypes = { Layer::TypeRaster };
		app->declareTool(_brushToolName, ToolInfo(text, icon, supportedTypes));
	}
	{
		QString text = QObject::tr("Brush Settings");
		app->declareSidebar(_brushSidebarName, SidebarInfo(text));
	}
}

}
