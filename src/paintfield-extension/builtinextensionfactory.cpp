#include "basictool/brush/brushtoolextension.h"
#include "basictool/move/layermovetoolextension.h"
#include "colorui/coloruiextension.h"
#include "layerui/layeruiextension.h"
#include "toolui/tooluiextension.h"
#include "navigator/navigatorextension.h"
#include "aboutdialog/aboutdialogextension.h"

#include "builtinextensionfactory.h"

namespace PaintField
{

BuiltInExtensionFactory::BuiltInExtensionFactory(QObject *parent) :
    ExtensionFactory(parent)
{
	addSubExtensionFactory(new BrushToolExtensionFactory(this));
	addSubExtensionFactory(new LayerMoveToolExtensionFactory(this));
	addSubExtensionFactory(new ColorUIExtensionFactory(this));
	addSubExtensionFactory(new LayerUIExtensionFactory(this));
	addSubExtensionFactory(new ToolUIExtensionFactory(this));
	addSubExtensionFactory(new NavigatorExtensionFactory(this));
	addSubExtensionFactory(new AboutDialogExtensionFactory(this));
}

}
