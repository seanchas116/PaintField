#include <QtPlugin>

#include "paintfield/core/appcontroller.h"
#include "paintfield/core/settingsmanager.h"
#include "paintfield/core/widgets/simplebutton.h"

#include "layermovetool.h"

#include "layermovetoolextension.h"

namespace PaintField
{

static const QString _layerMoveToolName = "paintfield.tool.move";

Tool *LayerMoveToolExtension::createTool(const QString &name, Canvas *canvas)
{
	if (name == _layerMoveToolName)
		return new LayerMoveTool(canvas);
	return 0;
}

void LayerMoveToolExtensionFactory::initialize(AppController *app)
{
	QString text = QObject::tr("Layer Move");
	QIcon icon = SimpleButton::createIcon(":/icons/24x24/move.svg");
	app->settingsManager()->declareTool(_layerMoveToolName, ToolInfo(text, icon, {"raster"}));
}

}

