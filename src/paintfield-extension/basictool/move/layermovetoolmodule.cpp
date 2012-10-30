#include "paintfield-core/application.h"
#include "paintfield-core/widgets/simplebutton.h"

#include "layermovetool.h"

#include "layermovetoolmodule.h"

namespace PaintField
{

const QString _layerMoveToolName = "paintfield.tool.move";

Tool *LayerMoveToolModule::createTool(const QString &name, CanvasView *view)
{
	if (name == _layerMoveToolName)
		return new LayerMoveTool(view);
	return 0;
}

void LayerMoveToolModuleFactory::initialize(Application *app)
{
	QString text = QObject::tr("Layer Move");
	QIcon icon = SimpleButton::createSimpleIconSet(":/icons/24x24/move.svg");
	QVector<int> supportedTypes = { Layer::TypeRaster };
	app->declareTool(_layerMoveToolName, ToolInfo(text, icon, supportedTypes));
}


}

