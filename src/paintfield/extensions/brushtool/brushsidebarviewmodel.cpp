#include "brushsidebarviewmodel.h"
#include "brushpresetmanager.h"
#include "paintfield/core/observablevariantmap.h"

namespace PaintField {

BrushSidebarViewModel::BrushSidebarViewModel(BrushPresetManager *presetManager, QObject *parent) :
	MVVMViewModel(parent),
	mPresetManager(presetManager)
{
	route(presetManager->parameters()->customProperty("size"), "size");
	route(presetManager->commonParameters()->customProperty("smooth"), "smooth");
	route(presetManager, "title", "title");
}


} // namespace PaintField
