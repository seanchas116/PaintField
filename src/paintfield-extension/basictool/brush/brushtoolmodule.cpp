#include "paintfield-core/application.h"
#include "paintfield-core/widgets/simplebutton.h"

#include "brushtool.h"
#include "brushsettingsidebar.h"

#include "brushtoolmodule.h"

namespace PaintField
{

const QString _brushToolName("paintfield.tool.brush");
const QString _brushSidebarName("paintfield.sidebar.brushSettings");

BrushToolModule::BrushToolModule(WorkspaceController *workspace, QObject *parent) :
	WorkspaceModule(workspace, parent)
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

QWidget *BrushToolModule::createSideBar(const QString &name)
{
	if (name == _brushSidebarName)
		return new BrushSettingSidebar(&_setting);
	return 0;
}

void BrushToolModuleFactory::initialize(Application *app)
{
	{
		QString text = QObject::tr("Brush");
		QIcon icon = SimpleButton::createSimpleIconSet(":/icons/24x24/brush.svg", QSize(24,24));
		QVector<int> supportedTypes = { Layer::TypeRaster };
		app->declareTool(_brushToolName, ToolDeclaration(text, icon, supportedTypes));
	}
	{
		QString text = QObject::tr("Brush Settings");
		app->declareSideBar(_brushSidebarName, SidebarDeclaration(text));
	}
}

}
