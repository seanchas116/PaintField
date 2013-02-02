#include "paintfield-core/appcontroller.h"
#include "paintfield-core/settingsmanager.h"
#include "paintfield-core/widgets/simplebutton.h"

#include "layermovetool.h"

#include "layermovetoolmodule.h"

namespace PaintField
{

const QString _layerMoveToolName = "paintfield.tool.move";

Tool *LayerMoveToolModule::createTool(const QString &name, Canvas *canvas)
{
	if (name == _layerMoveToolName)
		return new LayerMoveTool(canvas);
	return 0;
}

void LayerMoveToolModuleFactory::initialize(AppController *app)
{
	QString text = QObject::tr("Layer Move");
	QIcon icon = SimpleButton::createSimpleIconSet(":/icons/24x24/move.svg", QSize(24,24));
	app->settingsManager()->declareTool(_layerMoveToolName, ToolInfo(text, icon, {"raster"}));
}


}

