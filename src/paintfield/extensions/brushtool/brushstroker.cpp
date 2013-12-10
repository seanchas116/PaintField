
#include <Malachite/CurveSubdivision>

#include "brushstroker.h"

using namespace Malachite;

namespace PaintField {

BrushStroker::BrushStroker(Surface *surface) :
	mSurface(surface),
	mOriginalSurface(*surface)
{
}


QWidget *BrushStrokerFactory::createEditor(ObservableVariantMap *parameters)
{
	Q_UNUSED(parameters);
	return nullptr;
}

}

