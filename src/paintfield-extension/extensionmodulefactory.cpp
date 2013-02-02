#include "basictool/brush/brushtoolmodule.h"
#include "basictool/move/layermovetoolmodule.h"
#include "colorui/coloruimodule.h"
#include "layerui/layeruimodule.h"
#include "toolui/tooluimodule.h"
#include "navigator/navigatormodule.h"
#include "aboutdialog/aboutdialogmodule.h"

#include "extensionmodulefactory.h"

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
