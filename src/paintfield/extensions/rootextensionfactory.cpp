#include "aboutdialog/aboutdialogextension.h"
#include "brushtool/brushtoolextension.h"
#include "colorui/coloruiextension.h"
#include "layerui/layeruiextension.h"
#include "movetool/layermovetoolextension.h"
#include "navigator/navigatorextension.h"
#include "toolui/tooluiextension.h"
#include "vectortools/vectortoolsextension.h"
#include "formatsupports/formatsupportsextension.h"

#include "rootextensionfactory.h"

namespace PaintField {

RootExtensionFactory::RootExtensionFactory(QObject *parent) :
	ExtensionFactory(parent)
{
	addSubExtensionFactory(new AboutDialogExtensionFactory(this));
	addSubExtensionFactory(new BrushToolExtensionFactory(this));
	addSubExtensionFactory(new ColorUIExtensionFactory(this));
	addSubExtensionFactory(new LayerUIExtensionFactory(this));
	addSubExtensionFactory(new LayerMoveToolExtensionFactory(this));
	addSubExtensionFactory(new NavigatorExtensionFactory(this));
	addSubExtensionFactory(new ToolUIExtensionFactory(this));
	addSubExtensionFactory(new VectorToolsExtensionFactory(this));
	addSubExtensionFactory(new FormatSupportsExtensionFactory(this));
}

void RootExtensionFactory::initialize(AppController *app)
{
	Q_UNUSED(app)
}

} // namespace PaintField
