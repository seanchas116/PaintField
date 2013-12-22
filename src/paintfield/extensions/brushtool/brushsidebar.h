#pragma once

#include "paintfield/core/mvvmview.h"

namespace PaintField {

class BrushSideBar : public MVVMView
{
	Q_OBJECT
public:
	explicit BrushSideBar(QWidget *parent = 0);
};

} // namespace PaintField

