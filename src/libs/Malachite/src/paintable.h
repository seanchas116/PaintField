#pragma once

//ExportName: Paintable

#include "global.h"

namespace Malachite
{

class PaintEngine;

class MALACHITESHARED_EXPORT Paintable
{
public:
	virtual ~Paintable() {}
	virtual PaintEngine *createPaintEngine() = 0;
};

}

