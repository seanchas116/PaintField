#pragma once
#include "paintfield/core/mvvmviewmodel.h"

namespace PaintField {

class BrushPresetManager;

class BrushSidebarViewModel : public MVVMViewModel
{
	Q_OBJECT
public:
	explicit BrushSidebarViewModel(BrushPresetManager *presetManager, QObject *parent = 0);

private:
	BrushPresetManager *mPresetManager = nullptr;
};

} // namespace PaintField
