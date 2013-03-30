#include <QtPlugin>

#include "paintfield/core/settingsmanager.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/widgets/simplebutton.h"

#include "recttool.h"
#include "vectortoolsextension.h"

namespace PaintField {

VectorToolsExtension::VectorToolsExtension(Canvas *canvas, QObject *parent) :
	CanvasExtension(canvas, parent)
{
	
}

static const QString _rectToolName = "paintfield.tool.rectangle";

Tool *VectorToolsExtension::createTool(const QString &name, Canvas *canvas)
{
	if (name == _rectToolName)
		return new RectTool(canvas);
	
	return 0;
}

void VectorToolsExtensionFactory::initialize(AppController *app)
{
	{
		auto text = QObject::tr("Rectangle");
		auto icon = SimpleButton::createIcon(":/icons/24x24/rect.svg");
		app->settingsManager()->declareTool(_rectToolName, ToolInfo(text, icon, QStringList()));
	}
}

} // namespace PaintField

Q_EXPORT_PLUGIN2(paintfield-vectortools, PaintField::VectorToolsExtensionFactory)
