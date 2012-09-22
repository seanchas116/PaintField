#include "layertreeview.h"

namespace PaintField
{

void LayerTreeView::focusInEvent(QFocusEvent *event)
{
	Q_UNUSED(event);
	emit windowFocused();
}

}

