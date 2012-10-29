#include "basictool/brush/brushtoolmodule.h"
#include "basictool/move/layermovetoolmodule.h"
#include "colorui/coloruimodule.h"
#include "layerui/layeruimodule.h"
#include "toolui/tooluimodule.h"

#include "extensionmodulefactory.h"

namespace PaintField
{

ExtensionModuleFactory::ExtensionModuleFactory()
{
	addSubModuleFactory(new BrushToolModuleFactory);
	addSubModuleFactory(new LayerMoveToolModuleFactory);
	addSubModuleFactory(new ColorUIModuleFactory);
	addSubModuleFactory(new LayerUIModuleFactory);
	addSubModuleFactory(new ToolUIModuleFactory);
}

}
