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

ExtensionModuleFactory::ExtensionModuleFactory(QObject *parent) :
    ModuleFactory(parent)
{
	addSubModuleFactory(new BrushToolModuleFactory(this));
	addSubModuleFactory(new LayerMoveToolModuleFactory(this));
	addSubModuleFactory(new ColorUIModuleFactory(this));
	addSubModuleFactory(new LayerUIModuleFactory(this));
	addSubModuleFactory(new ToolUIModuleFactory(this));
	addSubModuleFactory(new NavigatorModuleFactory(this));
	addSubModuleFactory(new AboutDialogModuleFactory(this));
}

}
