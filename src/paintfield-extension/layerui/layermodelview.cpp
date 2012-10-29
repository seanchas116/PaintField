#include "layermodelview.h"

namespace PaintField
{

void LayerModelView::focusInEvent(QFocusEvent *event)
{
	Q_UNUSED(event);
	emit windowFocused();
}

}

